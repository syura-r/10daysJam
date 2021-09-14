#include "Object3D.h"
#include"DirectXLib.h"
#include "FBXModel.h"
Camera* Object3D::camera = nullptr;
LightCamera* Object3D::lightCamera = nullptr;
bool Object3D::drawShadow = false;
LightGroup* Object3D::lightGroup = nullptr;
Object3D * Object3D::Create(Model* model,Vector3& pos, Vector3& scale, Vector3& rot, XMFLOAT4& color)
{
	Object3D* object = new Object3D(pos, scale, rot, color);
	if (object == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!object->Initialize()) {
		delete object;
		assert(0);
	}

	if (model) {
		object->SetModel(model);
	}

	return object;
}

void Object3D::SetCamera(Camera * camera)
{
	Object3D::camera = camera;
}

Object3D::Object3D(Vector3 & pos, Vector3 & scale, Vector3 & rot, XMFLOAT4 & color)
	:position(pos),scale(scale),rotation(rot),color(color)
{
}

bool Object3D::Initialize()
{
	HRESULT result;
	// 定数バッファの生成
	result = DirectXLib::GetInstance()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));

	// 定数バッファの生成
	result = DirectXLib::GetInstance()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(LightCameraCBData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&lCameraConstBuff));
	assert(SUCCEEDED(result));

	
	return true;
}

void Object3D::Update(const BILLBOARD_TYPE billboardType)
{
	HRESULT result;
	XMMATRIX matScale, matRot, matTrans;
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	//ワールド行列の合成
	matWorld = XMMatrixIdentity();
	matWorld *= matScale;
	matWorld *= matRot;
	switch (billboardType)
	{
	case NORMAL:
	{
		const XMMATRIX& matBillboard = camera->GetMatBillboard();
		matWorld *= matBillboard;
		break;
	}
	case Y_AXIS:
	{
		const XMMATRIX& matBillboardY = camera->GetMatBillboardY();
		matWorld *= matBillboardY;
		break;
	}
	default:
		break;
	}
	matWorld *= matTrans;
	// 親オブジェクトがあれば
	if (parent != nullptr) {
		// 親オブジェクトのワールド行列を掛ける
		matWorld *= parent->matWorld;
	}	
}

void Object3D::WorldUpdate(const Vector3& rot,const BILLBOARD_TYPE billboardType)
{
	HRESULT result;
	XMMATRIX matScale, matTrans, matRot;
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX camMatWorld = XMMatrixInverse(nullptr, camera->GetMatView());

	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));

	appendRot *= XMMatrixRotationAxis(camMatWorld.r[2], rot.z);
	appendRot *= XMMatrixRotationAxis(camMatWorld.r[0], rot.x);
	appendRot *= XMMatrixRotationAxis(camMatWorld.r[1], rot.y);

	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	//ワールド行列の合成
	matWorld = XMMatrixIdentity();
	matWorld *= matScale;
	//XMMATRIX matRot2 = XMMatrixIdentity();
	//matRot2 *= XMMatrixRotationY(XMConvertToRadians(90));
	//matWorld *= matRot2;
	//matWorld *= matRot;
	switch (billboardType)
	{
	case NORMAL:
	{
		const XMMATRIX& matBillboard = camera->GetMatBillboard();
		matWorld *= matBillboard;
		break;
	}
	case Y_AXIS:
	{
		const XMMATRIX& matBillboardY = camera->GetMatBillboardY();
		matWorld *= matBillboardY;
		break;
	}
	default:
		break;
	}
	matWorld *= matTrans;
	// 親オブジェクトがあれば
	if (parent != nullptr) {
		// 親オブジェクトのワールド行列を掛ける
		matWorld *= parent->matWorld;
	}
}

void Object3D::Draw(const bool fbx, const bool shade, BLENDTYPE type, const bool customPipeline)
{
	HRESULT result;
	const XMMATRIX& matViewProjection = camera->GetMatViewProjection();
	const XMFLOAT3& cameraPos = camera->GetEye();
	auto cmdList = DirectXLib::GetInstance()->GetCommandList();
	if (drawShadow)
	{
		// 定数バッファへデータ転送
		LightCameraCBData* constMap = nullptr;
		result = lCameraConstBuff->Map(0, nullptr, (void**)&constMap);
		assert(SUCCEEDED(result));
		constMap->viewprojection = lightCamera->GetMatViewProjection();
		constMap->world = matWorld;
		lCameraConstBuff->Unmap(0, nullptr);

	}
	else
	{
		// 定数バッファへデータ転送
		ConstBufferData* constMap = nullptr;
		result = constBuff->Map(0, nullptr, (void**)&constMap);
		assert(SUCCEEDED(result));
		//constMap->mat = matWorld * matViewProjection;	// 行列の合成
		constMap->viewprojection = matViewProjection;
		constMap->cameraPos = cameraPos;
		constMap->world = matWorld;
		constMap->color = color;

		constBuff->Unmap(0, nullptr);

	}

	if (fbx)
	{
		if(drawShadow)
			PipelineState::SetPipeline("FBXShadowMap", ALPHA);
		else
		PipelineState::SetPipeline("FBX", type);
			
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else
	{
		if (!customPipeline)
		{
			if (drawShadow)
				PipelineState::SetPipeline("ShadowMap", ALPHA);
			else
			{
				if (!shade)
				{
					PipelineState::SetPipeline("NoShade", type);
				}
				else
				{
					PipelineState::SetPipeline("BasicObj", type);
				}
				cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			}
		}
	}

	if (drawShadow)
	// 定数バッファビューをセット
		cmdList->SetGraphicsRootConstantBufferView(0, lCameraConstBuff->GetGPUVirtualAddress());
	else
		cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	if (!drawShadow&&shade)
		//ライトの描画
		lightGroup->Draw(3);
	//モデルの描画
	model->Draw();
}

