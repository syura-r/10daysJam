#pragma once
#include "Scene.h"
#include"CreateFigure.h"
#include"Texture.h"
#include"Alpha.h"

#include "DebugCamera.h"
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

	static void SetCamera(DebugCamera* cameraPtr) { camera = cameraPtr; }
	static void SetWindowsize(Vector2 windowsize) { Title::windowsize = windowsize; }

private:
	static DebugCamera* camera;

	//ui
	SelectUI* selectUI = nullptr;
	static Vector2 windowsize;

	//obj
	//UnionPartsMotion* unionParts = nullptr;
};
