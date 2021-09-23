#include "Title.h"

#include "Audio.h"
#include "PtrDelete.h"
#include "Easing.h"

Title::Title()
{
	next = SCENE::Play;

	bg01 = new Sprite();
	bg02 = new Sprite();
	bg03 = new Sprite();
	selectUI = new SelectUI();
	sceneCh = new SceneChange();

	for (int i = 0; i < leafMax; i++)
	{
		leafs[i] = new Leaf();
		leafs[i]->sprite = new Sprite();
		float r = i % 4;
		if (r == 0.0f)
		{
			leafs[i]->texname = "leaf01";
		}
		else if (r == 1.0f)
		{
			leafs[i]->texname = "leaf02";
		}
		else if (r == 2.0f)
		{
			leafs[i]->texname = "leaf03";
		}
		else
		{
			leafs[i]->texname = "leaf04";
		}
	}
}


Title::~Title()
{
	PtrDelete(bg01);
	PtrDelete(bg02);
	PtrDelete(bg03);
	PtrDelete(selectUI);
	PtrDelete(sceneCh);
	for (int i = 0; i < leafMax; i++)
	{
		delete leafs[i]->sprite;
		delete leafs[i];
	}
}

void Title::Initialize()
{
	Audio::StopWave("endingBGM");
	Audio::PlayWave("titleBGM", 0.4f, true);
	Audio::StopWave("decision");

	isEnd = false;
	isAllEnd = false;

	firstBGPosX = 120.0f;
	easingCounter = 0;
	bg02_position = { firstBGPosX,0.0f };
	bg03_position = { -firstBGPosX,0.0f };
	selectUI->Initialize(SelectUI::State::title);
	sceneCh->Initialize();

	leafBackLine = 1080 + 64;
	for (int i = 0; i < leafMax; i++)
	{
		leafs[i]->position = { 0, leafBackLine };
	}
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
		//—t‚Á‚Ï‚Ì“®‚«
		for (int i = 0; i < leafMax; i++)
		{
			//–ß‚Á‚ÄÄÝ’è
			if (leafs[i]->position.y >= leafBackLine)
			{
				leafs[i]->position.x = (float)(std::rand() % 1980);
				leafs[i]->position.y = -(float)(std::rand() % 540) + 32;
				leafs[i]->rotation = (float)(std::rand() % 360);
				leafs[i]->speed = (float)(std::rand() % 3) + 0.5f;//0‰ñ”ð‚Ì+0.5f
			}
			leafs[i]->position.y += leafs[i]->speed;
			leafs[i]->rotation += 0.1f;
		}

	selectUI->Update();

	//Œˆ’è
	if ((Input::TriggerKey(DIK_SPACE) || Input::TriggerPadButton(XINPUT_GAMEPAD_A) || Input::TriggerPadButton(XINPUT_GAMEPAD_B)) &&
		sceneCh->GetToSmallEnd() &&
		!sceneCh->GetToBig())
	{
		Audio::PlayWave("decision", 1, true, 0);
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
	bg01->DrawSprite("TitleBackground_1", { 0.0f, 0.0f }, 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
}

void Title::PostDraw()
{
	sceneCh->Draw({ 0,0,0,1 });

	for (int i = 0; i < leafMax; i++)
	{
		leafs[i]->sprite->DrawSprite(leafs[i]->texname, leafs[i]->position, leafs[i]->rotation);
	}

	selectUI->Draw();

	bg03->DrawSprite("TitleBackground_3", bg03_position, 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
	bg02->DrawSprite("TitleBackground_2", bg02_position, 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
}
