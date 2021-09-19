#include "Title.h"
#include "PtrDelete.h"
#include "Easing.h"

Title::Title()
{
	next = SCENE::Play;

	bg01 = new Sprite();
	bg02 = new Sprite();
	bg03 = new Sprite();
	selectUI = new SelectUI();
	//unionParts = new UnionPartsMotion();
}


Title::~Title()
{
	PtrDelete(bg01);
	PtrDelete(bg02);
	PtrDelete(bg03);
	PtrDelete(selectUI);
	//PtrDelete(unionParts);
}

void Title::Initialize()
{
	isEnd = false;
	isAllEnd = false;

	firstBGPosX = 120.0f;
	easingCounter = 0;
	bg02_position = { firstBGPosX,0.0f };
	bg03_position = { -firstBGPosX,0.0f };
	selectUI->Initialize(SelectUI::State::title);
	//unionParts->Initialize();
}

void Title::Update()
{
	if(Input::TriggerKey(DIK_RETURN))
	ShutDown();

	//背景の動き
	int easingMax = 60 * 10;
	if (easingCounter < easingMax)
	{
		easingCounter++;
		bg02_position.x = Easing::EaseOutQuint(firstBGPosX, 0.0f, easingMax, easingCounter);
		bg03_position.x = -bg02_position.x;
	}

	selectUI->Update();
	//unionParts->Update();

	//決定
	if (Input::TriggerKey(DIK_SPACE) || Input::TriggerPadButton(XINPUT_GAMEPAD_A) || Input::TriggerPadButton(XINPUT_GAMEPAD_B))
	{
		if (selectUI->GetSelectNum() == 0)
		{
			//はじめるの処理
			ShutDown();
		}
		else
		{
			//おわるの処理
			isAllEnd = true;
		}
	}

}

void Title::PreDraw()
{
	selectUI->Draw();

	bg03->DrawSprite("TitleBackground_3", bg03_position, 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
	bg02->DrawSprite("TitleBackground_2", bg02_position, 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
	bg01->DrawSprite("TitleBackground_1", { 0.0f, 0.0f }, 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
}

void Title::PostDraw()
{
	//unionParts->Draw();

}
