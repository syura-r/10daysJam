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

	for (int i = 0; i < 2; i++)
	{
		bg01[i] = new Sprite();
		bg02[i] = new Sprite();
	}
}


Play::~Play()
{
	PtrDelete(lightGroup);

	objectManager->End();

	PtrDelete(plife);
	PtrDelete(sceneCh);
	for (int i = 0; i < 2; i++)
	{
		PtrDelete(bg01[i]);
		PtrDelete(bg02[i]);
	}

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

	Vector3 effectPos = { 5,2,5 };
	float rotation = 90;
	Vector3 color = { 1,0,0 };
	if (Input::TriggerKey(DIK_P))
	{
		ParticleEmitter::CreateSlashPerfect(effectPos, rotation, color);
	}
	if (Input::TriggerKey(DIK_L))
	{
		ParticleEmitter::CreateRiseEffects(effectPos, color);
	}
	if (Input::TriggerKey(DIK_O))
	{
		ParticleEmitter::CreateWindEffects(effectPos, color);
	}
	if (Input::TriggerKey(DIK_K))
	{
		ParticleEmitter::CreateSparkEffects(effectPos, color);
	}
#endif // _DEBUG

	plife->Update(3);

	sceneCh->Update();

	//画面スクロール
	{
		float speed = 1.0f;//プレイヤーの移動量を入れる
		bg01_position[0].x -= speed / 3;
		if (bg01_position[0].x < -1920)
		{
			bg01_position[0].x = 0;
		}
		bg01_position[1].x = bg01_position[0].x + 1920;


		bg02_position[0].x -= speed;
		if (bg02_position[0].x < -1920)
		{
			bg02_position[0].x = 0;
		}
		bg02_position[1].x = bg02_position[0].x + 1920;
	}

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
	plife->Draw();

	objectManager->PostDraw();

	for (int i = 0; i < 2; i++)
	{
		bg02[i]->DrawSprite("TitleBackground_2", bg02_position[i], 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
		bg01[i]->DrawSprite("TitleBackground_1", bg01_position[i], 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
	}

}
