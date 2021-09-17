#include "Title.h"
#include "PtrDelete.h"
#include "Easing.h"

DebugCamera* Title::camera = nullptr;
Vector2 Title::windowsize = Vector2{ 0,0 };

Title::Title()
{
	next = SCENE::Play;

	selectUI = new SelectUI(windowsize);
	//unionParts = new UnionPartsMotion(camera);
}


Title::~Title()
{
	PtrDelete(selectUI);
	//PtrDelete(unionParts);
}

void Title::Initialize()
{
	isEnd = false;

	selectUI->Initialize(SelectUI::State::title);
	//unionParts->Initialize();
}

void Title::Update()
{
	if(Input::TriggerKey(DIK_RETURN))
	ShutDown();

	//選択
	selectUI->Update();
	//決定
	if (Input::TriggerKey(DIK_SPACE) || Input::CheckPadButton(XINPUT_GAMEPAD_A) || Input::CheckPadButton(XINPUT_GAMEPAD_B))
	{
		if (selectUI->GetSelectNum() == 0)
		{
			//はじめるの処理
		}
		else
		{
			//おわるの処理
		}
	}

	//チェック用
	if (Input::TriggerKey(DIK_1))
	{
		selectUI->Initialize(SelectUI::State::title);
	}
	if (Input::TriggerKey(DIK_2))
	{
		selectUI->Initialize(SelectUI::State::gameover);
	}
	if (Input::TriggerKey(DIK_3))
	{
		selectUI->Initialize(SelectUI::State::gameclear);
	}


	//unionParts->Update();
}

void Title::PreDraw()
{
}

void Title::PostDraw()
{
	selectUI->Draw();
	//unionParts->Draw();
}
