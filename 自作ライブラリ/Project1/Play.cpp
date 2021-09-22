#include "Play.h"
#include"PtrDelete.h"
#include"Object3D.h"
#include"Audio.h"
#include "Boss.h"
#include "Enemy.h"
#include"ParticleEmitter.h"
#include"SphereCollider.h"
#include"imgui.h"
#include"FBXManager.h"
#include "Input.h"
#include "MapBox.h"
#include "OBJLoader.h"
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

	Player* player = new Player();
	Enemy::SetPlayer(player);
	Boss::SetPlayer(player);
	CreateStage();
	objectManager->Add(player);

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

	sceneCh->Initialize();

	bg01_position[0] = { 0,0 };
	bg01_position[1] = { 0,0 };
	bg02_position[0] = { 0,0 };
	bg02_position[1] = { 0,0 };
}

void Play::Update()
{
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

	sceneCh->Update();

	//画面スクロール
	{
		float windowX = 1920.0f;
		float speed = 1.0f;//プレイヤーの移動量を入れる
		bg01_position[0].x -= speed / 3.0f;
		if (bg01_position[0].x < -windowX)
		{
			bg01_position[0].x = 0.0f;
		}
		bg01_position[1].x = bg01_position[0].x + windowX;


		bg02_position[0].x -= speed;
		if (bg02_position[0].x < -windowX)
		{
			bg02_position[0].x = 0.0f;
		}
		bg02_position[1].x = bg02_position[0].x + windowX;
	}

	if (sceneCh->GetToBigEnd())
	{
		ShutDown();
	}
}

void Play::PreDraw()
{
	if (!Object3D::GetDrawShadow())
	{
		ImGui::Begin("Light");
		ImGui::SliderFloat3("LightDir", lightDir, -1.0f, 1.0f);
		ImGui::Text("Camera Rotation : RightMouseButton + Drag");
		ImGui::Text("Camera Zoom     : MouseWheel");
		ImGui::Text("Camera Reset    : R");
		ImGui::End();
		Object3D::GetLightCamera()->SetLightDir({ lightDir[0],lightDir[1] ,lightDir[2] });
	}



	for (int i = 0; i < 2; i++)
	{
		bg02[i]->DrawSprite("Play_Background_2", bg02_position[i], 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
	}
	for (int i = 0; i < 2; i++)
	{
		bg01[i]->DrawSprite("Play_Background_1", bg01_position[i], 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
	}
	//objectManager->PostDraw();


}

void Play::PostDraw()
{
	sceneCh->Draw({ 0,0,0,1 });
	objectManager->PreDraw();

}

void Play::CreateStage()
{
	Enemy* enemy = new Enemy({ 20,9,0 });
	objectManager->Add(enemy);

	Boss* boss = new Boss({ 55,10,0 });
	objectManager->Add(boss);

	
	int map[15][60] = {
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,2,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	};

	for(int y = 0;y<15;y++)
	{
		for(int x = 0;x<60;x++)
		{
			if(map[y][x] == 1)
			{
				MapBox* object = new MapBox(OBJLoader::GetModel("box"), Vector3{ (float)x + 1.0f , 20.0f - y, 0 }, Vector3{ 1.0f,1.0f,1.0f },{},{0.5f,0.5f,0.5f});
				//TouchAbleObject* object = new TouchAbleObject();
				//object->Initialize(OBJLoader::GetModel("box"), Vector3{ (float)x + 1.0f , 15.0f - y, 0 }, Vector3{ 1.0f,1.0f,1.0f });
				objectManager->Add(object);
			}
			else if(map[y][x] == 2)
			{
				MapBox* object = new MapBox(OBJLoader::GetModel("stand"), Vector3{ (float)x + 1.0f , 20.0f - y, 0 }, Vector3{ 1.0f,1.0f,1.0f },{},{1,0.5f,0.5f});
				//TouchAbleObject* object = new TouchAbleObject();
				//object->Initialize(OBJLoader::GetModel("box"), Vector3{ (float)x + 1.0f , 15.0f - y, 0 }, Vector3{ 1.0f,1.0f,1.0f });
				objectManager->Add(object);
			}
		}
	}
}
