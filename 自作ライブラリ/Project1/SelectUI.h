#pragma once
#include "Sprite.h"

class SelectUI
{
public:
	enum State
	{
		title,
		gameclear,
		gameover,
	};

	SelectUI();
	~SelectUI();

	void Initialize(State state);
	void Update();
	void Draw();

	void SetState(State state);

	int GetSelectNum() { return selectNum; }

	static void SetWindowSize(XMFLOAT2 windowsize) { SelectUI::windowsize = windowsize; }

private:
	State state;

	//�薼
	Sprite* main = nullptr;
	XMFLOAT2 mainPosition;
	std::string mainTexname;

	struct Select
	{
		Sprite* sprite = nullptr;
		XMFLOAT2 position;
		XMFLOAT2 scale;
		XMFLOAT4 color;
		std::string texname;

		Select()
		{
			sprite = new Sprite();
		}
		~Select()
		{
			delete sprite;
		}
	};

	//�I��ԍ�
	int selectNum;
	//�I������
	Select* select1;
	Select* select2;

	static XMFLOAT2 windowsize;

};

