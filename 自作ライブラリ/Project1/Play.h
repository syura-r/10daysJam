#pragma once
#include "Scene.h"
#include"LightGroup.h"
#include"Object.h"
#include"CollisionManager.h"
#include "ComputeShade.h"
#include "NormalMapTestOBJ.h"

#include "GSShaderTestObj.h"
#include"Grass.h"
#include "ObjectManager.h"
#include "Sprite.h"
#include "SceneChange.h"
#include "PlayBackGround.h"
#include "DebugCamera.h"

class TouchAbleObject;
class Play :public Scene
{
public:
	Play();
	~Play()override;
	void Initialize()override;
	void Update()override;
	void PreDraw()override;
	void PostDraw()override;

	static void SetDebugCamera(DebugCamera* cameraPtr) { camera = cameraPtr; }

private:
	void CreateStage();
	LightGroup* lightGroup = nullptr;
	XMFLOAT3 pos = { 1,0,0 };
	float coloramb[3] = { 1,1,1 };
	float color0[3] = { 1,1,1 };
	float color1[3] = { 1,1,1 };
	float color2[3] = { 1,1,1 };
	float lightDir[3] = {0,-1,0};
	CollisionManager* collisionManager = nullptr;
	NormalMapTestOBJ* testNormalMapOBJ = nullptr;
	ObjectManager* objectManager = nullptr;

	SceneChange* sceneCh = nullptr;

	PlayBackGround* playbg = nullptr;

	static DebugCamera* camera;
};

