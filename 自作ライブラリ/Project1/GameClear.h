#pragma once
#include "Scene.h"
#include"CreateFigure.h"
#include"Texture.h"
#include"Alpha.h"

#include "SceneChange.h"
#include "SelectUI.h"
#include "Sprite.h"
#include "Object.h"
#include "DebugCamera.h"

class GameClear :public Scene
{
public:
	GameClear();
	~GameClear()override;
	void Initialize() override;
	void Update()override;
	void PreDraw()override;
	void PostDraw()override;

	static void SetDebugCamera(DebugCamera* cameraPtr) { camera = cameraPtr; }

private:
	SceneChange* sceneCh = nullptr;
	SelectUI* selectUI = nullptr;
	bool isSelectUIView;

	Sprite* bg = nullptr;
	Object* moji = nullptr;

	static DebugCamera* camera;
};
