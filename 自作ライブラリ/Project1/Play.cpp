#include "Play.h"
#include"PtrDelete.h"
#include"Object3D.h"
#include"Audio.h"
#include"ParticleEmitter.h"
#include"SphereCollider.h"
#include"imgui.h"
#include"FBXManager.h"
#include "Input.h"
#include "Player.h"

Play::Play()
{
	next = Ending;
	//ライト生成
	lightGroup = LightGroup::Create();
	//3Dオブジェクトにライトをセット
	Object3D::SetLightGroup(lightGroup);
	//ライト色を設定
	lightGroup->SetDirLightActive(0, true);
	lightGroup->SetDirLightColor(0, XMFLOAT3(color0));

	collisionManager = CollisionManager::GetInstance();
	objectManager = ObjectManager::GetInstance();

	objectManager->Add(new Player());

	selectUI = new SelectUI();
}


Play::~Play()
{
	PtrDelete(lightGroup);

	objectManager->End();

	PtrDelete(selectUI);
}

void Play::Initialize()
{
	isEnd = false;
	isAllEnd = false;

	selectUI->Initialize(SelectUI::State::gameclear);
	isGameover = false;
	isGameclear = false;
}

void Play::Update()
{
	//if(Input::TriggerKey(DIK_LSHIFT))
	//{
	//	Input::ChangeMouseMove();
	//}
	lightGroup->SetAmbientColor(XMFLOAT3(coloramb));
	lightGroup->SetDirLightDir(0, { lightDir[0],lightDir[1],lightDir[2],1 });
	lightGroup->Update();
	objectManager->Update();
	collisionManager->CheckAllCollisions();
	selectUI->Update();


	//ゲームオーバー
	if (isGameover)
	{
		selectUI->SetState(SelectUI::State::gameover);
	}
	//ゲームクリア
	if (isGameclear)
	{
		selectUI->SetState(SelectUI::State::gameclear);
	}

	//決定
	if (Input::TriggerKey(DIK_SPACE) || Input::CheckPadButton(XINPUT_GAMEPAD_A) || Input::CheckPadButton(XINPUT_GAMEPAD_B))
	{
		//ゲームオーバー
		if (isGameover)
		{
			if (selectUI->GetSelectNum() == 0)
			{
				//はじめから
				Initialize();
			}
			else
			{
				//タイトルに戻る
				next = SCENE::Title;
				isEnd = true;
			}
		}
		//ゲームクリア
		else if (isGameclear)
		{
			//タイトルに戻る
			next = SCENE::Title;
			isEnd = true;
		}
	}
}

void Play::PreDraw()
{
	if (Object3D::GetDrawShadow())
	{
		ImGui::Begin("Light");
		ImGui::SliderFloat3("LightDir", lightDir, -1.0f, 1.0f);
		ImGui::Text("NormalMap : 1");
		ImGui::Text("Bloom     : 2");
		ImGui::Text("Camera Rotation : RightMouseButton + Drag");
		ImGui::Text("Camera Zoom     : MouseWheel");
		ImGui::Text("Camera Reset    : R");
		ImGui::End();
		Object3D::GetLightCamera()->SetLightDir({ lightDir[0],lightDir[1] ,lightDir[2] });
	}
	objectManager->PreDraw();
}

void Play::PostDraw()
{
	objectManager->PostDraw();
	if (isGameover || isGameclear)
	{
		selectUI->Draw();
	}
}
