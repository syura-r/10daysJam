#pragma once
#include "Sprite.h"

class Player;

class PlayBackGround
{
public:
	PlayBackGround();
	~PlayBackGround();

	void Initialize();
	void Update();
	void Draw();

	void SetPlayer(Player* playerPtr) { this->player = playerPtr; }

private:
	Sprite* bg01[2];
	XMFLOAT2 bg01_position[2];

	Sprite* bg02[2];
	XMFLOAT2 bg02_position[2];

	Player* player = nullptr;
	XMFLOAT3 player_PrePos;

};

