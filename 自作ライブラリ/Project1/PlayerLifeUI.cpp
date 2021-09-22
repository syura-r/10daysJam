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
	nowLifeCount = playerLife + 1;
}

void PlayerLifeUI::Draw()
{
	XMFLOAT2 basePosition = { 50, 20 };

	for (int i = 0; i < nowLifeCount; i++)
	{
		sprites[i]->DrawSprite("HPber_0" + std::to_string(i), basePosition, 0.0f, { 1.0f,1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f,0.0f });
	}
}
