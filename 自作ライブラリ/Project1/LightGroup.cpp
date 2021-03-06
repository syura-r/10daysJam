#include "LightGroup.h"
#include"DirectXLib.h"
using namespace DirectX;

LightGroup * LightGroup::Create()
{
	LightGroup* instance = new LightGroup();

	instance->Initialize();

	return instance;
}

void LightGroup::Initialize()
{
	HRESULT result;
	auto dev = DirectXLib::GetInstance()->GetDevice();
	//定数バッファ
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff)&~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	if (FAILED(result)) { assert(0); }

	//定数バッファへデータ転送
	TransferConstBuff();

}

void LightGroup::TransferConstBuff()
{
	HRESULT result;
	//定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		//環境光
		constMap->ambientColor = ambientColor;
		//平行光源
		for (int i = 0; i < DirLightNum; i++) {
			// ライトが有効なら設定を転送
			if (dirLights[i].IsActive()) {
				constMap->dirLights[i].lightv = -dirLights[i].GetLightDir();
				constMap->dirLights[i].lightcolor = dirLights[i].GetLightColor();
			}
			// ライトが無効ならライト色を0に
			else {
				constMap->dirLights[i].lightv = { 0,0,0,0 };
				constMap->dirLights[i].lightcolor = { 0,0,0 };
			}
		}
		//点光源
		for (int i = 0; i < PointLightNum; i++)
		{
			//ライトが有効なら設定を転送
			if (pointLights[i].IsActive())
			{
				constMap->pointLights[i].active = 1;
				constMap->pointLights[i].lightpos = pointLights[i].GetLightPos();
				constMap->pointLights[i].lightcolor = pointLights[i].GetLightColor();
				constMap->pointLights[i].lightatten = pointLights[i].GetLightAtten();
			}
			else
			{
				constMap->pointLights[i].active = 0;
			}
		}
		//スポットライト
		for (int i = 0; i < SpotLightNum; i++)
		{
			//ライトが有効なら設定を転送
			if (spotLights[i].IsActive())
			{
				constMap->spotLights[i].active = 1;
				constMap->spotLights[i].lightv = -spotLights[i].GetLightDir();
				constMap->spotLights[i].lightpos = spotLights[i].GetLightPos();
				constMap->spotLights[i].lightcolor = spotLights[i].GetLightColor();
				constMap->spotLights[i].lightatten = spotLights[i].GetLightAtten();
				constMap->spotLights[i].lightfactoranglecos = spotLights[i].GetLightFactorAngleCos();
			}
			else
			{
				constMap->pointLights[i].active = 0;
			}
		}
		//丸影
		for (int i = 0; i < CircleShadowNum; i++)
		{
			//ライトが有効なら設定を転送
			if (circleShadows[i].IsActive())
			{
				constMap->circleShadows[i].active = 1;
				constMap->circleShadows[i].dir				   = -circleShadows[i].GetDir();
				constMap->circleShadows[i].casterpos		   = circleShadows[i].GetCasterPos();
				constMap->circleShadows[i].distanceCasterLight = circleShadows[i].GetDistanceCasterLight();
				constMap->circleShadows[i].atten			   = circleShadows[i].GetAtten();
				constMap->circleShadows[i].factorAngleCos	   = circleShadows[i].GetFactorAngleCos();
			}
			else
			{
				constMap->circleShadows[i].active = 0;

			}
		}

		constBuff->Unmap(0, nullptr);
	}
}

void LightGroup::SetAmbientColor(const XMFLOAT3 & color)
{
	ambientColor = color;
	dirty = true;
}

void LightGroup::SetDirLightActive(int index, bool active)
{
	assert(0 <= index && index < DirLightNum);
	dirLights[index].SetActive(active);
}

void LightGroup::SetDirLightDir(const int & index, const XMVECTOR & lightdir)
{
	assert(0 <= index && index < DirLightNum);
	dirLights[index].SetLightDir(lightdir);
	dirty = true;
}

void LightGroup::SetDirLightColor(const int & index, const XMFLOAT3 lightcolor)
{
	assert(0 <= index && index < DirLightNum);
	dirLights[index].SetLightColor(lightcolor);
	dirty = true;
}

void LightGroup::SetPointLightActive(int index, bool active)
{
	assert(0 <= index && index < PointLightNum);
	pointLights[index].SetActive(active);

}

void LightGroup::SetPointLightPos(int index, const XMFLOAT3 & pos)
{
	assert(0 <= index && index < PointLightNum);
	pointLights[index].SetLightPos(pos);
	dirty = true;
}

void LightGroup::SetPointLightColor(int index, const XMFLOAT3 & color)
{
	assert(0 <= index && index < PointLightNum);
	pointLights[index].SetLightColor(color);
	dirty = true;
}

void LightGroup::SetPointLightAtten(int index, const XMFLOAT3 & atten)
{
	assert(0 <= index && index < PointLightNum);
	pointLights[index].SetLightAtten(atten);
	dirty = true;
}

void LightGroup::SetSpotLightActive(int index, bool active)
{
	assert(0 <= index && index < SpotLightNum);
	spotLights[index].SetActive(active);
}

void LightGroup::SetSpotLightDir(int index, const XMVECTOR & dir)
{
	assert(0 <= index && index < SpotLightNum);
	spotLights[index].SetLightDir(dir);
	dirty = true;
}

void LightGroup::SetSpotLightPos(int index, const XMFLOAT3 & pos)
{
	assert(0 <= index && index < SpotLightNum);
	spotLights[index].SetLightPos(pos);
	dirty = true;
}

void LightGroup::SetSpotLightColor(int index, const XMFLOAT3 & color)
{
	assert(0 <= index && index < SpotLightNum);
	spotLights[index].SetLightColor(color);
	dirty = true;
}

void LightGroup::SetSpotLightAtten(int index, const XMFLOAT3 & atten)
{
	assert(0 <= index && index < SpotLightNum);
	spotLights[index].SetLightAtten(atten);
	dirty = true;
}

void LightGroup::SetSpotLightFactorAngle(int index, const XMFLOAT2 & lightFactorAngle)
{
	assert(0 <= index && index < SpotLightNum);
	spotLights[index].SetLightFactorAngle(lightFactorAngle);
	dirty = true;
}

void LightGroup::SetCircleShadowActive(int index, bool active)
{
	assert(0 <= index && index < CircleShadowNum);
	circleShadows[index].SetActive(active);
}

void LightGroup::SetCircleShadowCasterPos(int index, const XMFLOAT3 & pos)
{
	assert(0 <= index && index < CircleShadowNum);
	circleShadows[index].SetCasterPos(pos);
	dirty = true;
}

void LightGroup::SetCircleShadowDir(int index, const XMVECTOR & dir)
{
	assert(0 <= index && index < CircleShadowNum);
	circleShadows[index].SetDir(dir);
	dirty = true;
}

void LightGroup::SetCircleShadowDistanceCasterLight(int index, float distanceCasterLight)
{
	assert(0 <= index && index < CircleShadowNum);
	circleShadows[index].SetDistanceCasterLight(distanceCasterLight);
	dirty = true;
}

void LightGroup::SetCircleShadowAtten(int index, const XMFLOAT3 & atten)
{
	assert(0 <= index && index < CircleShadowNum);
	circleShadows[index].SetAtten(atten);
	dirty = true;
}

void LightGroup::SetCircleShadowFactorAngle(int index, const XMFLOAT2 & factorAngle)
{
	assert(0 <= index && index < CircleShadowNum);
	circleShadows[index].SetFactorAngle(factorAngle);
	dirty = true;
}

void LightGroup::Update()
{
	if (dirty)
	{
		TransferConstBuff();
		dirty = false;
	}
}

void LightGroup::Draw(UINT rootParameterIndex)
{
	DirectXLib::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(rootParameterIndex,
		constBuff->GetGPUVirtualAddress());
}
