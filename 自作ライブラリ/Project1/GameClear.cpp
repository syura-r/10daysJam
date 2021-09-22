#include "GameClear.h"
#include "PtrDelete.h"
#include "OBJLoader.h"

GameClear::GameClear()
{
	next = Title;
	sceneCh = new SceneChange();
	selectUI = new SelectUI();
	bg = new Sprite();
	moji = new Object();
	moji->Create(OBJLoader::GetModel("End"));
}

GameClear::~GameClear()
{
	PtrDelete(sceneCh);
	PtrDelete(selectUI);
	PtrDelete(bg);
	PtrDelete(moji);

}

void GameClear::Initialize()
{
	isEnd = false;
	isAllEnd = false;
	sceneCh->Initialize();
	selectUI->Initialize(SelectUI::State::gameclear);
	moji->SetPosition({ 5,2,5, });
	moji->SetRotation({ 90,0,0 });
	moji->SetScale({ 5,5,5 });
}

void GameClear::Update()
{
	if ((Input::TriggerKey(DIK_SPACE) || Input::TriggerPadButton(XINPUT_GAMEPAD_A) || Input::TriggerPadButton(XINPUT_GAMEPAD_B)) &&
		sceneCh->GetToSmallEnd() &&
		!sceneCh->GetToBig())
	{
		//タイトルに戻る
		next = SCENE::Title;
		sceneCh->ChangeStart();
	}
	if (sceneCh->GetToBigEnd())
	{
		ShutDown();
	}

	sceneCh->Update();
	selectUI->Update();

	//完起こし
	if (moji->GetRotation().x > 0.0f)
	{
		Vector3 rot = moji->GetRotation();
		rot.x -= 90.0f / 180.0f;
		moji->SetRotation(rot);
	}
	moji->Update();
}

void GameClear::PreDraw()
{
	sceneCh->Draw({ 0,0,0,1 });
	//selectUI->Draw();
	//bg->DrawSprite("gameover_bg", { 0,0 }, 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
}

void GameClear::PostDraw()
{
	moji->CustomDraw();
}
