#include "PlayerLifeUI.h"

PlayerLifeUI::PlayerLifeUI()
{
	for (int i = 0; i < spriteCount; i++)
	{
		sprites[i] = new Sprite();
	}
}

PlayerLifeUI::~PlayerLifeUI()
{
	for (int i = 0; i < spriteCount; i++)
	{
		delete sprites[i];
	}
}

void PlayerLifeUI::Initialize()
{
}

void PlayerLifeUI::Update(int playerLife)
{
	nowLifeCount = playerLife;
}

void PlayerLifeUI::Draw()
{
	for (int i = 0; i < nowLifeCount; i++)
	{
		float onesizeX = 128.0f;
		XMFLOAT2 position = { 10 + (onesizeX * i), 10 };
		sprites[i]->DrawSprite("player_life", position, 0.0f, { 1.0f,1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f,0.0f });
	}
}
