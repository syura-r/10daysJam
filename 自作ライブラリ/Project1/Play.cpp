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
	//���C�g����
	lightGroup = LightGroup::Create();
	//3D�I�u�W�F�N�g�Ƀ��C�g���Z�b�g
	Object3D::SetLightGroup(lightGroup);
	//���C�g�F��ݒ�
	lightGroup->SetDirLightActive(0, true);
	lightGroup->SetDirLightColor(0, XMFLOAT3(color0));

	collisionManager = CollisionManager::GetInstance();
	objectManager = ObjectManager::GetInstance();

	objectManager->Add(new Player());

	selectUI = new SelectUI();

	plife = new PlayerLifeUI();

	sceneCh = new SceneChange();

}


Play::~Play()
{
	PtrDelete(lightGroup);

	objectManager->End();

	PtrDelete(selectUI);
	PtrDelete(plife);
	PtrDelete(sceneCh);

}

void Play::Initialize()
{
	isEnd = false;
	isAllEnd = false;

	selectUI->Initialize(SelectUI::State::gameclear);
	isGameover = false;
	isGameclear = false;

	plife->Initialize();
	sceneCh->Initialize();

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


#ifdef _DEBUG
	if (Input::TriggerKey(DIK_1))
	{
		isGameover = true;
		isGameclear = false;
	}
	if (Input::TriggerKey(DIK_2))
	{
		isGameclear= true;
		isGameover = false;
	}
#endif // _DEBUG

	//�Q�[���I�[�o�[
	if (isGameover)
	{
		selectUI->SetState(SelectUI::State::gameover);
	}
	//�Q�[���N���A
	if (isGameclear)
	{
		selectUI->SetState(SelectUI::State::gameclear);
	}

	//����
	if (Input::TriggerKey(DIK_SPACE) || Input::TriggerPadButton(XINPUT_GAMEPAD_A) || Input::TriggerPadButton(XINPUT_GAMEPAD_B))
	{
		//�Q�[���I�[�o�[
		if (isGameover)
		{
			if (selectUI->GetSelectNum() == 0)
			{
				//�͂��߂���
				Initialize();
			}
			else
			{
				//�^�C�g���ɖ߂�
				next = SCENE::Title;
				sceneCh->ChangeStart();
			}
		}
		//�Q�[���N���A
		else if (isGameclear)
		{
			//�^�C�g���ɖ߂�
			next = SCENE::Title;
			sceneCh->ChangeStart();
		}
	}

	plife->Update(3);

	sceneCh->Update();

	if (sceneCh->GetToBigEnd())
	{
		ShutDown();
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
	sceneCh->Draw();

	objectManager->PreDraw();
}

void Play::PostDraw()
{
	objectManager->PostDraw();
	if (isGameover || isGameclear)
	{
		selectUI->Draw();
	}
	plife->Draw();
}
