#include "GameClear.h"
#include "PtrDelete.h"

GameClear::GameClear()
{
	next = Title;
	sceneCh = new SceneChange();
	selectUI = new SelectUI();
	bg = new Sprite();
}

GameClear::~GameClear()
{
	PtrDelete(sceneCh);
	PtrDelete(selectUI);
	PtrDelete(bg);

}

void GameClear::Initialize()
{
	isEnd = false;
	isAllEnd = false;
	sceneCh->Initialize();
	selectUI->Initialize(SelectUI::State::gameclear);
}

void GameClear::Update()
{
	if (Input::TriggerKey(DIK_SPACE) || Input::TriggerPadButton(XINPUT_GAMEPAD_A) || Input::TriggerPadButton(XINPUT_GAMEPAD_B))
	{
		//ƒ^ƒCƒgƒ‹‚É–ß‚é
		next = SCENE::Title;
		sceneCh->ChangeStart();
	}
	if (sceneCh->GetToBigEnd())
	{
		ShutDown();
	}

	sceneCh->Update();
	selectUI->Update();
}

void GameClear::PreDraw()
{
	sceneCh->Draw({ 0,0,0,1 });
	selectUI->Draw();
	//bg->DrawSprite("gameover_bg", { 0,0 }, 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
}

void GameClear::PostDraw()
{
}
