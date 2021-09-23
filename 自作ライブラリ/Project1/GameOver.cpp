#include "GameOver.h"

#include "Audio.h"
#include "PtrDelete.h"

GameOver::GameOver()
{
	next = Title;
	sceneCh = new SceneChange();
	selectUI = new SelectUI();
	bg = new Sprite();
}

GameOver::~GameOver()
{
	PtrDelete(sceneCh);
	PtrDelete(selectUI);
	PtrDelete(bg);

}

void GameOver::Initialize()
{
	Audio::StopWave("playBGM");
	Audio::StopWave("bossBGM");
	Audio::PlayWave("gameOver",1,true,1);
	isEnd = false;
	isAllEnd = false;
	sceneCh->Initialize();
	selectUI->Initialize(SelectUI::State::gameover);
}

void GameOver::Update()
{
	if ((Input::TriggerKey(DIK_SPACE) || Input::TriggerPadButton(XINPUT_GAMEPAD_A) || Input::TriggerPadButton(XINPUT_GAMEPAD_B)) &&
		sceneCh->GetToSmallEnd() &&
		!sceneCh->GetToBig())
	{
		Audio::PlayWave("decision", 1, true, 0);
		if (selectUI->GetSelectNum() == 0)
		{
			//�͂��߂���
			next = SCENE::Play;
		}
		else
		{
			//�^�C�g���ɖ߂�
			next = SCENE::Title;
		}
		sceneCh->ChangeStart();
	}
	if (sceneCh->GetToBigEnd())
	{
		ShutDown();
	}

	sceneCh->Update();
	selectUI->Update();
}

void GameOver::PreDraw()
{
	sceneCh->Draw({ 1,1,1,1 });
	selectUI->Draw();
	bg->DrawSprite("gameover_bg", { 0,0 }, 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
}

void GameOver::PostDraw()
{
}
