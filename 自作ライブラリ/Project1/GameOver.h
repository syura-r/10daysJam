#pragma once
#include "Scene.h"
#include"CreateFigure.h"
#include"Texture.h"
#include"Alpha.h"

#include "SceneChange.h"
#include "SelectUI.h"
#include "Sprite.h"
class GameOver :public Scene
{
public:
	GameOver();
	~GameOver()override;
	void Initialize() override;
	void Update()override;
	void PreDraw()override;
	void PostDraw()override;

private:
	SceneChange* sceneCh = nullptr;
	SelectUI* selectUI = nullptr;
	Sprite* bg = nullptr;
};
