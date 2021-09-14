#include "GSShaderTestObj.h"
#include"OBJLoader.h"
GSShaderTestObj::GSShaderTestObj(const Vector3& pos)
{
	Create(OBJLoader::GetModel("sphere"));
	position = pos;
	destruction = 0;
	scaleFactor = 0.57f;
	positionFactor = 0.32f;
	rotationFactor = 0.79f;
	tessellation = 1;

	CreateConstBuff();
}


void GSShaderTestObj::Draw()
{
	if (!Object3D::GetDrawShadow())
	{

		ImGui::Begin("Status");
		ImGui::SliderFloat("destruction", &destruction, 0, 1.0f);
		ImGui::SliderFloat("scaleFactor", &scaleFactor, 0, 1.0f);
		ImGui::SliderFloat("positionFactor", &positionFactor, 0, 2.0f);
		ImGui::SliderFloat("rotationFactor", &rotationFactor, 0, 30.0f);
		ImGui::SliderInt("tessellation", &tessellation, 1, 32);

		ImGui::End();
	}
	//// 定数バッファへデータ転送
	ConstBuffData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->_Destruction = destruction;
	constMap->_ScaleFactor = scaleFactor;
	constMap->_PositionFactor = positionFactor;
	constMap->_RotationFactor = rotationFactor;
	constMap->_Tessellation = tessellation;


	constBuff->Unmap(0, nullptr);

	//floor->Draw();
	DirectXLib::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	if(Object3D::GetDrawShadow())
	{
		// パイプラインステートの設定
		PipelineState::SetPipeline("PBShadowMap");
		DirectXLib::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, constBuff->GetGPUVirtualAddress());

	}
	else
	{
		// パイプラインステートの設定
		PipelineState::SetPipeline("PolygonBreak");
		DirectXLib::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(4, constBuff->GetGPUVirtualAddress());
	}
	CustomDraw(false,true, ALPHA, true);
}

void GSShaderTestObj::CreateConstBuff()
{
	HRESULT result = DirectXLib::GetInstance()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBuffData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
}
