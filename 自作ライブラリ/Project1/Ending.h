#pragma once
#include "Scene.h"
#include"CreateFigure.h"
#include"Texture.h"
#include"Alpha.h"

#include "SceneChange.h"
class Ending :public Scene
{
public:
	Ending();
	~Ending()override;
	void Initialize() override;
	void Update()override;
	void PreDraw()override;
	void PostDraw()override;
private:
	SceneChange* sceneCh = nullptr;
};

