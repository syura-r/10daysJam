#include "PlayBackGround.h"
#include "PtrDelete.h"
#include "Player.h"

PlayBackGround::PlayBackGround()
{
	for (int i = 0; i < 2; i++)
	{
		bg01[i] = new Sprite();
		bg02[i] = new Sprite();
	}
}

PlayBackGround::~PlayBackGround()
{
	for (int i = 0; i < 2; i++)
	{
		PtrDelete(bg01[i]);
		PtrDelete(bg02[i]);
	}
}

void PlayBackGround::Initialize()
{
	bg01_position[0] = { 0,0 };
	bg01_position[1] = { 0,0 };
	bg02_position[0] = { 0,0 };
	bg02_position[1] = { 0,0 };

	player_PrePos = player->GetPosition();
}

void PlayBackGround::Update()
{
	//‰æ–ÊƒXƒNƒ[ƒ‹
	if (player->GetPosition().x > 11 &&
		!player->GetStartFight())
	{
		float windowX = 1920.0f;
		float speed = player_PrePos.x - player->GetPosition().x;

		bg01_position[0].x += speed * 20;
		if (bg01_position[0].x < -windowX ||
			bg01_position[0].x > 0.0f)
		{
			bg01_position[0].x = 0.0f;
		}
		bg01_position[1].x = bg01_position[0].x + windowX;


		bg02_position[0].x += speed * 40;
		if (bg02_position[0].x < -windowX ||
			bg02_position[0].x > 0.0f)
		{
			bg02_position[0].x = 0.0f;
		}
		bg02_position[1].x = bg02_position[0].x + windowX;
	}
	player_PrePos = player->GetPosition();
}


void PlayBackGround::Draw()
{
	for (int i = 0; i < 2; i++)
	{
		bg02[i]->DrawSprite("Play_Background_2", bg02_position[i], 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
	}
	for (int i = 0; i < 2; i++)
	{
		bg01[i]->DrawSprite("Play_Background_1", bg01_position[i], 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
	}
}
