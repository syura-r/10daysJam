#include "Title.h"
#include "PtrDelete.h"

Title::Title()
{
	next = SCENE::Play;

	selectUI = new SelectUI();
	//unionParts = new UnionPartsMotion();
}


Title::~Title()
{
	PtrDelete(selectUI);
	//PtrDelete(unionParts);
}

void Title::Initialize()
{
	isEnd = false;
	isAllEnd = false;

	selectUI->Initialize(SelectUI::State::title);
	//unionParts->Initialize();
}

void Title::Update()
{
	if(Input::TriggerKey(DIK_RETURN))
	ShutDown();

	selectUI->Update();
	//unionParts->Update();

	//決定
	if (Input::TriggerKey(DIK_SPACE) || Input::CheckPadButton(XINPUT_GAMEPAD_A) || Input::CheckPadButton(XINPUT_GAMEPAD_B))
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
}

void Title::PostDraw()
{
	selectUI->Draw();
	//unionParts->Draw();
}
