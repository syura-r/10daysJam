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
	//ui
	SelectUI* selectUI = nullptr;
	//obj
	//UnionPartsMotion* unionParts = nullptr;
};
