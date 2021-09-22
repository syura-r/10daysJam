#pragma once
#include <Effekseer.h>
#include <EffekseerRendererDX12.h>
#include <unordered_map> 
#include <string>
#include "Camera.h"
#include <stdio.h>
#include <DirectXMath.h>

class EffekseerLib
{
	static std::unordered_map<std::string, Effekseer::Effect*> effects;
	//エフェクトマネージャー
	static Effekseer::Manager* efkManager;
	static bool allStop;
private:
	//エフェクトレンダラー
	EffekseerRenderer::Renderer* efkRenderer = nullptr;
	//メモリプール
	EffekseerRenderer::SingleFrameMemoryPool* efkMemoryPool = nullptr;
	//コマンドリスト
	EffekseerRenderer::CommandList* efkCmdList = nullptr;

	Camera* camera = nullptr;

	
public:
	EffekseerLib();
	~EffekseerLib();
	void Initialize(Camera* camera);
	void Update();
	void BeginRendering();
	void Draw();
	static void Draw(Effekseer::Handle & handle);
	void EndRendering();
	void Load(const std::string& name, const std::string& effectName);
	static void PlayEffect(const std::string & name, Effekseer::Handle & handle, const DirectX::XMFLOAT3 pos);
	static void PlayEffectTest(const std::string & name, Effekseer::Handle & handle, const DirectX::XMFLOAT3 pos);
	static void PlayEffectTest2(const std::string & name, Effekseer::Handle & handle, const DirectX::XMFLOAT3 pos);
	static bool GetEnd(Effekseer::Handle& handle);
	static void StopEffect(Effekseer::Handle & handle);
	static void SetPosition(Effekseer::Handle& handle, DirectX::XMFLOAT3 pos);
	static void SetRotationX(Effekseer::Handle& handle, float rad);
	static void SetRotationY(Effekseer::Handle& handle, float rad);
	static void SetRotationZ(Effekseer::Handle& handle, float rad);
	static void SetScale(Effekseer::Handle& handle, float scale);
	static void SetColor(Effekseer::Handle& handle, DirectX::XMFLOAT4 color);
	static void StopAllEffect();
	static void StopTrue() { allStop = true; }
	static void Stopfalse() { allStop = false; }
	void SyncronizeEffekseerCamera();
};

