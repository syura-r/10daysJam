#pragma once
#include "Scene.h"
#include"LightGroup.h"
#include"Object.h"
#include"Player.h"
#include"CollisionManager.h"
#include "ComputeShade.h"
#include "NormalMapTestOBJ.h"

#include "GSShaderTestObj.h"
#include"Grass.h"
#include "ObjectManager.h"
#include "Sprite.h"
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
private:
	Grass* grass = nullptr;
	GSShaderTestObj* testObj = nullptr;
	//TouchAbleObject* floor = nullptr;
	LightGroup* lightGroup = nullptr;
	XMFLOAT3 pos = { 1,0,0 };
	float coloramb[3] = { 1,1,1 };
	float color0[3] = { 1,1,1 };
	float color1[3] = { 1,1,1 };
	float color2[3] = { 1,1,1 };
	float lightDir[3] = {0,-1,0};
	bool targetSphere = false;
	//Player* player = nullptr;
	CollisionManager* collisionManager = nullptr;
	Object* testBloomOBJ = nullptr;
	NormalMapTestOBJ* testNormalMapOBJ = nullptr;
	Sprite* shadowTex = nullptr;
	ObjectManager* objectManager = nullptr;
	//ComputeShade* testParticle = nullptr;
};

