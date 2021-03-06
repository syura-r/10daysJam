#include "SelectUI.h"

#include "Audio.h"
#include "PtrDelete.h"
#include "Input.h"

XMFLOAT2 SelectUI::windowsize = { 0.0f,0.0f };

SelectUI::SelectUI()
{
	main = new Sprite();
	select1 = new Select();
	select2 = new Select();
}

SelectUI::~SelectUI()
{
	PtrDelete(main);
	PtrDelete(select1);
	PtrDelete(select2);
}

void SelectUI::Initialize(State state)
{
	selectNum = 0;
	SetState(state);
	preSelectNum = 0;
}

void SelectUI::Update()
{
	//選択
	XMFLOAT2 selectScale = { 1.0f,1.0f };
	XMFLOAT2 notSelectScale = { 0.8f,0.8f };
	XMFLOAT4 selectColor = { 1,1,1,1 };
	XMFLOAT4 notSelectColor = { 0.5f,0.5f,0.5f,1.0f };
	preSelectNum = selectNum;
	if (selectNum == 0)
	{
		//大きさ
		select1->scale = selectScale;
		select2->scale = notSelectScale;
		//色
		select1->color = selectColor;
		select2->color = notSelectColor;
	}
	else if (selectNum == 1)
	{
		//大きさ
		select1->scale = notSelectScale;
		select2->scale = selectScale;
		//色
		select1->color = notSelectColor;
		select2->color = selectColor;
	}

	//上選択
	if (Input::TriggerKey(DIK_UP) || Input::TriggerKey(DIK_W) || Input::CheckPadLStickUp() || Input::CheckPadButton(XINPUT_GAMEPAD_DPAD_UP))
	{
		selectNum = 0;
	}
	//下選択
	if (Input::TriggerKey(DIK_DOWN) || Input::TriggerKey(DIK_S) || Input::CheckPadLStickDown() || Input::CheckPadButton(XINPUT_GAMEPAD_DPAD_DOWN))
	{
		selectNum = 1;
	}

	//選択肢が1つなので固定
	if (state == State::gameclear)
	{
		selectNum = 0;
	}

	if(selectNum != preSelectNum)
	{
		Audio::PlayWave("select",0.5f);
	}
}

void SelectUI::Draw()
{
	if (state != State::gameclear)
	{
		main->DrawSprite(mainTexname, mainPosition, 0.0f, { 1.0f, 1.0f });
	}
	select1->sprite->DrawSprite(select1->texname, select1->position, 0.0f, select1->scale, select1->color);
	if (state != State::gameclear)
	{
		select2->sprite->DrawSprite(select2->texname, select2->position, 0.0f, select2->scale, select2->color);
	}
}

void SelectUI::SetState(State state)
{
	this->state = state;

	switch (state)
	{
	case SelectUI::title:
		mainTexname = "title_logo";
		select1->texname = "title_start";
		select2->texname = "title_end";

		mainPosition = { windowsize.x / 2.0f, windowsize.y / 14.0f * 5.0f };
		select1->position = { windowsize.x / 2.0f, windowsize.y / 14.0f * 10.0f };
		select2->position = { windowsize.x / 2.0f, windowsize.y / 14.0f * 12.0f };

		break;

	case SelectUI::gameclear:
		select1->texname = "result_totitle";

		select1->position = { windowsize.x / 5.0f * 4.0f, windowsize.y / 7.0f * 6.0f };
		break;

	case SelectUI::gameover:
		mainTexname = "gameover_main";
		select1->texname = "gameover_restart";
		select2->texname = "gameover_totitle";

		mainPosition = { windowsize.x / 4.0f, windowsize.y / 43.0f * 13.0f };
		select1->position = { windowsize.x / 4.0f, windowsize.y / 43.0f * 28.0f };
		select2->position = { windowsize.x / 4.0f, windowsize.y / 43.0f * 34.0f };
		break;

	default:
		break;
	}
}
