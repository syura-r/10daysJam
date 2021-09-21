#pragma once
#include "Sprite.h"

class PlayerLifeUI
{
public:
	PlayerLifeUI();
	~PlayerLifeUI();

	void Initialize();
	void Update(int playerLife);
	void Draw();

private:
	static const int spriteCount = 4;//playerÇÃç≈ëÂHP+1
	Sprite* sprites[spriteCount];

	int nowLifeCount;
};

