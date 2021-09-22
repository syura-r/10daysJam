#include "Ending.h"
#include "PtrDelete.h"

Ending::Ending()
{
	next = Title;
	sceneCh = new SceneChange();

}


Ending::~Ending()
{
	PtrDelete(sceneCh);

}

void Ending::Initialize()
{
	isEnd = false;
	isAllEnd = false;
	sceneCh->Initialize();

}

void Ending::Update()
{
	if (Input::TriggerKey(DIK_SPACE) || Input::TriggerPadButton(XINPUT_GAMEPAD_A) || Input::TriggerPadButton(XINPUT_GAMEPAD_B))
	{
		sceneCh->ChangeStart();
	}
	if (sceneCh->GetToBigEnd())
	{
		ShutDown();
	}

	sceneCh->Update();

}

void Ending::PreDraw()
{
	sceneCh->Draw({ 1,1,1,1 });

}

void Ending::PostDraw()
{
}
