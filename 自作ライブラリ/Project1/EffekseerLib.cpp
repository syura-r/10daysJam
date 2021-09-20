#include "EffekseerLib.h"
#include "DirectXLib.h"

#ifdef _DEBUG
#pragma comment(lib, "EffekseerRendererDX12d.lib")
#pragma comment(lib, "Effekseerd.lib")
#pragma comment(lib, "LLGId.lib")
#endif

#ifndef _DEBUG
#pragma comment(lib, "EffekseerRendererDX12.lib")
#pragma comment(lib, "Effekseer.lib")
#pragma comment(lib, "LLGI.lib")
#endif

//#pragma comment(lib, "EffekseerSoundXAudio2.lib")
//#pragma comment(lib, "EffekseerSoundXAudio2d.lib")

std::unordered_map<std::string, Effekseer::Effect*> EffekseerLib::effects = {};
Effekseer::Manager* EffekseerLib::efkManager = nullptr;
bool EffekseerLib::allStop = false;

EffekseerLib::EffekseerLib()
{
}

EffekseerLib::~EffekseerLib()
{
	efkRenderer->Destroy();
	efkManager->Destroy();
	ES_SAFE_RELEASE(efkCmdList);
	ES_SAFE_RELEASE(efkMemoryPool);
	for (auto itr = effects.begin(); itr != effects.end();++itr) 
	{
		ES_SAFE_RELEASE(itr->second);
	}
	effects.clear();
}


void EffekseerLib::Initialize(Camera* camera)
{
	this->camera = camera;
	ID3D12Device * dev = DirectXLib::GetInstance()->GetDevice();
	ID3D12CommandQueue* cmdQue = DirectXLib::GetInstance()->GetCmdQueue();
	DXGI_FORMAT bbFormats[] = { DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM };
	DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	efkRenderer = EffekseerRendererDX12::Create(
		dev,
		cmdQue,
		3,
		&Format,
		1,
		DXGI_FORMAT_UNKNOWN,
		false,
		10000
	);
	efkManager = Effekseer::Manager::Create(10000);

	efkManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
	// 描画用インスタンスから描画機能を設定
	efkManager->SetSpriteRenderer(efkRenderer->CreateSpriteRenderer());
	efkManager->SetRibbonRenderer(efkRenderer->CreateRibbonRenderer());
	efkManager->SetRingRenderer(efkRenderer->CreateRingRenderer());
	efkManager->SetTrackRenderer(efkRenderer->CreateTrackRenderer());
	efkManager->SetModelRenderer(efkRenderer->CreateModelRenderer());

	// 描画用インスタンスからテクスチャの読込機能を設定
	// 独自拡張可能
	efkManager->SetTextureLoader(efkRenderer->CreateTextureLoader());
	efkManager->SetModelLoader(efkRenderer->CreateModelLoader());

	//DX12特有の処理
	efkMemoryPool = EffekseerRendererDX12::CreateSingleFrameMemoryPool(efkRenderer);
	efkCmdList = EffekseerRendererDX12::CreateCommandList(efkRenderer, efkMemoryPool);
	efkRenderer->SetCommandList(efkCmdList);

	SyncronizeEffekseerCamera();
}

void EffekseerLib::Load(const std::string& name, const std::string& effectName)
{
	// ファイルパスを結合
	std::string filePath = "lib/Effect/" + effectName + ".efk";
	std::string materialPath = "lib/Effect/";
	wchar_t wfilepath[128];
	wchar_t wmaterialpath[128];

	// ユニコード文字列に変換
	MultiByteToWideChar(CP_ACP, 0, filePath.c_str(), -1, wfilepath, _countof(wfilepath));
	MultiByteToWideChar(CP_ACP, 0, materialPath.c_str(), -1, wmaterialpath, _countof(wmaterialpath));
	effects[name] = Effekseer::Effect::Create(efkManager, (const EFK_CHAR*)wfilepath, 1.0f);
}

void EffekseerLib::PlayEffect(const std::string & name,Effekseer::Handle & handle, const DirectX::XMFLOAT3 pos)
{
	if (!efkManager->Exists(handle))
	{
		handle = efkManager->Play(EffekseerLib::effects[name], pos.x, pos.y, pos.z);
	}
}

void EffekseerLib::PlayEffectTest(const std::string & name, Effekseer::Handle & handle, const DirectX::XMFLOAT3 pos)
{
	efkManager->StopEffect(handle);
	handle = efkManager->Play(EffekseerLib::effects[name], pos.x, pos.y, pos.z);
}

void EffekseerLib::PlayEffectTest2(const std::string & name, Effekseer::Handle & handle, const DirectX::XMFLOAT3 pos)
{
	handle = efkManager->Play(EffekseerLib::effects[name], pos.x, pos.y, pos.z);
}



void EffekseerLib::StopEffect(Effekseer::Handle & handle)
{
	efkManager->StopEffect(handle);
	//efkManager->StopRoot(handle);
}

void EffekseerLib::SetPosition(Effekseer::Handle& handle,DirectX::XMFLOAT3 pos)
{
	if (efkManager->Exists(handle))
	{
		efkManager->SetLocation(handle, { pos.x,pos.y,pos.z });
	}
}

void EffekseerLib::SetRotationX(Effekseer::Handle& handle, float rad)
{
	efkManager->SetRotation(handle, { 1,0,0 }, rad);
}

void EffekseerLib::SetRotationY(Effekseer::Handle& handle, float rad)
{
	efkManager->SetRotation(handle, { 0,1,0 }, rad);
}

void EffekseerLib::SetRotationZ(Effekseer::Handle& handle, float rad)
{
	efkManager->SetRotation(handle, { 0,0,1 }, rad);
	//efkManager->DrawHandle(handle);
}

void EffekseerLib::SetScale(Effekseer::Handle& handle, float scale)
{
	efkManager->SetScale(handle, scale, scale, scale);
}

void EffekseerLib::SetColor(Effekseer::Handle & handle, DirectX::XMFLOAT4 color)
{
	Effekseer::Color c;
	c.A = color.w;
	c.R = color.x;
	c.G = color.y;
	c.B = color.z;
	efkManager->SetAllColor(handle, c);
}

void EffekseerLib::StopAllEffect()
{
	efkManager->StopAllEffects();
	int a = 0;
}

bool EffekseerLib::GetEnd(Effekseer::Handle& handle)
{
	return efkManager->Exists(handle);
}

void EffekseerLib::SyncronizeEffekseerCamera()
{
	Effekseer::Matrix44 fkViewMat;
	Effekseer::Matrix44 fkProjMat;

	auto view = camera->GetMatView();
	auto proj = camera->GetMatProjection();

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			fkViewMat.Values[i][j] = view.r[i].m128_f32[j];
			fkProjMat.Values[i][j] = proj.r[i].m128_f32[j];
		}
	}

	efkRenderer->SetCameraMatrix(fkViewMat);
	efkRenderer->SetProjectionMatrix(fkProjMat);
}


void EffekseerLib::Update()
{
	SyncronizeEffekseerCamera();
	efkManager->Update();
	efkMemoryPool->NewFrame();
}

void EffekseerLib::BeginRendering()
{
	EffekseerRendererDX12::BeginCommandList(efkCmdList, DirectXLib::GetInstance()->GetCommandList());
	efkRenderer->BeginRendering();
}

void EffekseerLib::Draw()
{
	if (allStop == false)
	{
		efkManager->Draw();
	}
	
}

void EffekseerLib::EndRendering()
{
	efkRenderer->EndRendering();
	EffekseerRendererDX12::EndCommandList(efkCmdList);
}

void EffekseerLib::Draw(Effekseer::Handle & handle)
{
	efkManager->DrawHandle(handle);
}
