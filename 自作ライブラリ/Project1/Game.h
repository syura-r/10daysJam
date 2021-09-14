#pragma once
#include <d3d12.h>
#include<dxgi1_6.h>
#include <d3dcompiler.h>
#include<DirectXTex.h>
#include<wrl.h>
#include"Input.h"
#include"window.h"
#include"DirectXLib.h"
#include"Game.h"
#include"Camera.h"
#include "DebugCamera.h"
#include "LightCamera.h"
#include "PostEffect.h"
#include"SceneManager.h"
#include "ShadowMap.h"
#include "Sprite.h"
#include"Vector.h"
using namespace Microsoft::WRL;
using namespace DirectX;

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
class DirectXLib;
class Game
{
public:
	static Game* GetInstance();
	~Game();
	void Initialize();
	void Run();
	void End();
private:
	Game();
	static Game* instance;
	void RoadAsset();
	void CreatePipeline();
	void LoadFinish();
	void DrawLoadTex();
	SceneManager* sceneManeger = nullptr;
	Window* win = nullptr;
	DirectXLib* directX = nullptr;
	DebugCamera* camera = nullptr;
	Vector3 cameraPos;
	PostEffect* postEffect = nullptr;
	LightCamera* lightCamera = nullptr;
	ShadowMap* shadowMap = nullptr;
	float dir[3] = { 0.0f,-1.0f,0.0f };
	float distance = 10;

	//ÉçÅ[ÉhíÜ
	bool nowLoading;

	Sprite* loadTex = nullptr;
	Sprite* loadDot = nullptr;

	int loadAssetLevel = 0;
	int createPipelineLevel = 0;

	bool loadAssetFinish = false;
	bool createPipelineFinish = false;
};

