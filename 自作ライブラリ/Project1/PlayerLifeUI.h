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
	static const int spriteCount = 3;//player�̍ő�HP
	Sprite* sprites[spriteCount];

	int nowLifeCount;
};

