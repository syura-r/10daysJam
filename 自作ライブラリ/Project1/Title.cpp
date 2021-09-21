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
	sceneCh = new SceneChange();
}


Title::~Title()
{
	PtrDelete(bg01);
	PtrDelete(bg02);
	PtrDelete(bg03);
	PtrDelete(selectUI);
	//PtrDelete(unionParts);
	PtrDelete(sceneCh);
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
	sceneCh->Initialize();
}

void Title::Update()
{
	if(Input::TriggerKey(DIK_RETURN))
	ShutDown();

	//”wŒi‚Ì“®‚«
	int easingMax = 60 * 10;
	if (easingCounter < easingMax)
	{
		easingCounter++;
		bg02_position.x = Easing::EaseOutQuint(firstBGPosX, 0.0f, easingMax, easingCounter);
		bg03_position.x = -bg02_position.x;
	}

	selectUI->Update();
	//unionParts->Update();

	//Œˆ’è
	if ((Input::TriggerKey(DIK_SPACE) || Input::TriggerPadButton(XINPUT_GAMEPAD_A) || Input::TriggerPadButton(XINPUT_GAMEPAD_B)) &&
		sceneCh->GetToSmallEnd() &&
		!sceneCh->GetToBig())
	{
		if (selectUI->GetSelectNum() == 0)
		{
			//‚Í‚¶‚ß‚é‚Ìˆ—
			sceneCh->ChangeStart();
		}
		else
		{
			//‚¨‚í‚é‚Ìˆ—
			isAllEnd = true;
		}
	}
	sceneCh->Update();

	if (sceneCh->GetToBigEnd())
	{
		ShutDown();
	}
}

void Title::PreDraw()
{
	sceneCh->Draw({ 0,0,0,1 });

	selectUI->Draw();

	bg03->DrawSprite("TitleBackground_3", bg03_position, 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
	bg02->DrawSprite("TitleBackground_2", bg02_position, 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
	bg01->DrawSprite("TitleBackground_1", { 0.0f, 0.0f }, 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
}

void Title::PostDraw()
{
	//unionParts->Draw();

}
