#include "FBXModel.h"
#include"DirectXLib.h"
#include "FbxLoader.h"
#include "Object3D.h"
#include "Texture.h"
#include"PtrDelete.h"
void FBXModel::CreateBuffers()
{
	auto dev = DirectXLib::GetInstance()->GetDevice();
	HRESULT result;
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataSkin) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffSkin));
	//頂点データ全体ｎサイズ
	UINT sizeVB =
		static_cast<UINT>(sizeof(VertexPosNormalUvSkin) *
			vertices.size());

	//頂点バッファの生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	//頂点バッファへのデータ転送
	VertexPosNormalUvSkin* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if(SUCCEEDED(result))
	{
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	//頂点バッファビュー(VBV)の作成
	vbView.BufferLocation =
		vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

	//頂点インデックス全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short)*indices.size());
	//インデックスバッファ作成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	//インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**) &indexMap);
	if(SUCCEEDED(result))
	{
		std::copy(indices.begin(), indices.end(), indexMap);
		indexBuff->Unmap(0, nullptr);
	}
	//インデックスバッファビュー(IBV)の作成
	ibView.BufferLocation =
		indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB ;

	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	for (int i = 0; i < MAX_BONES; i++)
	{
		constMapSkin->bones[i] = XMMatrixIdentity();
	}
	constBuffSkin->Unmap(0, nullptr);

	
	frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);
	//テクスチャ画像データ
	//const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);//生データ抽出
	//assert(img);

	//リソース設定
	//CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
	//	metadata.format,
	//	metadata.width,
	//	(UINT)metadata.height,
	//	(UINT16)metadata.arraySize,
	//	(UINT16)metadata.mipLevels);

	//テクスチャ用バッファの作成
	//result = DirectXLib::GetDevice()->CreateCommittedResource(
	//	&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
	//		D3D12_MEMORY_POOL_L0),
	//	D3D12_HEAP_FLAG_NONE,
	//	&texresDesc,
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&texBuff));
}

FBXModel::~FBXModel()
{
	//fbxScene->Destroy();
}

void FBXModel::Draw()
{
	//static int a = 0;
	//a++;
	//if(a == 1)
	//if(isPlay)
	//{
	//	currentTime += frameTime;
	//	if(currentTime>endTime)
	//	{
	//		currentTime = startTime;
	//	}
	//}
	//定数バッファのデータ転送
	ConstBufferDataSkin* constMapSkin = nullptr;
	HRESULT result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	for(int i = 0;i<bones.size();i++)
	{
		//今の姿勢
		XMMATRIX matCurrentPose;
		//今の姿勢行列を取得
		FbxAMatrix fbxCurrentPose =
			bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);
		//XMMATRIXに変換
		FbxLoader::ConvertMatrixFromFbx(&matCurrentPose,fbxCurrentPose);
		//合成してスキニングして行列に
		constMapSkin->bones[i] = bones[i].invInitialPose * matCurrentPose;
	}
	constBuffSkin->Unmap(0, nullptr);
	
	auto cmdList = DirectXLib::GetInstance()->GetCommandList();
	//頂点バッファをセット(VBV)
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	//インデックスバッファをセット
	cmdList->IASetIndexBuffer(&ibView);

	//デスクリプタヒープのセット
	ID3D12DescriptorHeap* ppHeaps[] = { Texture::GetBasicDescHeap().Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	if (!Object3D::GetDrawShadow())
	{
		//シェーダーリソースビューをセット
		cmdList->SetGraphicsRootDescriptorTable(2, Texture::GetGpuDescHandleSRV(texName));
		cmdList->SetGraphicsRootConstantBufferView(4, constBuffSkin->GetGPUVirtualAddress());
	}
	else
		cmdList->SetGraphicsRootConstantBufferView(1, constBuffSkin->GetGPUVirtualAddress());
	//描画コマンド
	cmdList->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);

	
}

void FBXModel::AnimationInit()
{
	//0番のアニメーション取得
	FbxAnimStack* animstack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	//アニメーションの名前取得
	const char* animstackname = animstack->GetName();
	//アニメションの時間取得
	FbxTakeInfo* takeinfo = fbxScene->GetTakeInfo(animstackname);

	//開始時間取得
	startTime = takeinfo->mLocalTimeSpan.GetStart();
	//終了時間取得
	endTime = takeinfo->mLocalTimeSpan.GetStop();
	//開始時間に合わせる
	currentTime = startTime;
	//再生中状態にする
	isPlay = true;
}

void FBXModel::SetAnimationFrame(const int startFrame, const int endFrame, const int FrameTime)
{
	startTime.SetTime(0, 0, 0, startFrame, 0, FbxTime::EMode::eFrames60);
	currentTime = startTime;
	endTime.SetTime(0, 0, 0, endFrame, 0, FbxTime::EMode::eFrames60);
	if(startFrame > endFrame)
		frameTime.SetTime(0, 0, 0, -FrameTime, 0, FbxTime::EMode::eFrames60);
	else
		frameTime.SetTime(0, 0, 0, FrameTime, 0, FbxTime::EMode::eFrames60);
	isPlay = true;
}

bool FBXModel::PlayAnimation(bool endless)
{
	if (!isPlay)
		return false;
	
	currentTime += frameTime;
	if ((currentTime > endTime && frameTime > 0) || (currentTime < endTime && frameTime < 0))
	{
		currentTime = startTime;
		if (!endless)
		{
			isPlay = false;
			return false;
		}
	}


	return true;
}
