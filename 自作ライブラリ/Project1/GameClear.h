#pragma once
#include "Scene.h"
#include"CreateFigure.h"
#include"Texture.h"
#include"Alpha.h"

#include "SceneChange.h"
#include "SelectUI.h"
#include "Sprite.h"
class GameClear :public Scene
{
public:
	GameClear();
	~GameClear()override;
	void Initialize() override;
	void Update()override;
	void PreDraw()override;
	void PostDraw()override;

private:
	SceneChange* sceneCh = nullptr;
	SelectUI* selectUI = nullptr;
	Sprite* bg = nullptr;
};
