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
	next = GameClear;
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

	plife = new PlayerLifeUI();

	sceneCh = new SceneChange();

}


Play::~Play()
{
	PtrDelete(lightGroup);

	objectManager->End();

	PtrDelete(plife);
	PtrDelete(sceneCh);

}

void Play::Initialize()
{
	isEnd = false;
	isAllEnd = false;

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


#ifdef _DEBUG
	if (Input::TriggerKey(DIK_1) &&
		sceneCh->GetToSmallEnd() &&
		!sceneCh->GetToBig())
	{
		next = GameOver;
		sceneCh->ChangeStart();

	}
	if (Input::TriggerKey(DIK_2) &&
		sceneCh->GetToSmallEnd() &&
		!sceneCh->GetToBig())
	{
		next = GameClear;
		sceneCh->ChangeStart();
	}

	if (Input::TriggerKey(DIK_P))
	{
		Vector3 effectPos = { 5,2,5 };
		ParticleEmitter::CreateSlashEffects(effectPos);
		ParticleEmitter::CreateSparkEffects(effectPos);
		ParticleEmitter::CreateShockEffects(effectPos);
	}
	if (Input::TriggerKey(DIK_O))
	{
		Vector3 effectPos = { 5,2,5 };
		ParticleEmitter::CreateRiseEffects(effectPos);
	}
#endif // _DEBUG

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

	sceneCh->Draw({ 0,0,0,1 });

	objectManager->PreDraw();
}

void Play::PostDraw()
{
	objectManager->PostDraw();
	plife->Draw();
}
