#pragma once
#include "Scene.h"
#include"CreateFigure.h"
#include"Texture.h"
#include"Alpha.h"

#include "SelectUI.h"
#include "UnionPartsMotion.h"

class Title :
	public Scene
{
public:
	Title();
	~Title()override;
	void Initialize() override;
	void Update()override;
	void PreDraw()override;
	void PostDraw()override;

private:
	//background
	Sprite* bg01 = nullptr;
	Sprite* bg02 = nullptr;
	Sprite* bg03 = nullptr;

	XMFLOAT2 bg02_position;
	XMFLOAT2 bg03_position;

	float firstBGPosX;
	int easingCounter;

	//ui
	SelectUI* selectUI = nullptr;
	//obj
	//UnionPartsMotion* unionParts = nullptr;
};
