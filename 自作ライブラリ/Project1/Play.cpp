#include "Play.h"
#include"PtrDelete.h"
#include"Object3D.h"
#include"Audio.h"
#include "Boss.h"
#include "Enemy.h"
#include"SphereCollider.h"
#include"imgui.h"
#include"FBXManager.h"
#include "Input.h"
#include "MapBox.h"
#include "OBJLoader.h"
#include "Player.h"

DebugCamera* Play::camera = nullptr;
bool Play::isGameClear = false;
bool Play::isGameOver = false;

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

	 player = new Player();
	 
	 boss = new Boss({ 50,15,0 });

	Enemy::SetPlayer(player);
	Boss::SetPlayer(player);
	CreateStage();
	sceneCh = new SceneChange();

	playbg = new PlayBackGround();
	playbg->SetPlayer(player);

	for(int i = 0;i<3;i++)
	{
		lectureTex[i] = new Object();
		std::string modelName = "Lecture_0" + std::to_string(i + 1);
		lectureTex[i]->Create(FBXManager::GetModel(modelName));
		lectureTex[i]->SetScale({3,3,3});
	}
	lectureTex[0]->SetPosition({ 5,8,1 });
	lectureTex[1]->SetPosition({ 15,8,1 });
	lectureTex[2]->SetPosition({ 25,8,1 });
	for (int i = 0; i < 3; i++)
	{
		lectureTex[i]->Update();
	}
}


Play::~Play()
{
	PtrDelete(lightGroup);

	objectManager->End();

	PtrDelete(player);
	PtrDelete(boss);
	//PtrDelete(plife);
	PtrDelete(sceneCh);
	PtrDelete(playbg);
	for (int i = 0; i < 3; i++)
	{
		PtrDelete(lectureTex[i]);
	}

}

void Play::Initialize()
{
	Audio::StopWave("gameOver");
	Audio::StopWave("decision");
	Audio::StopWave("titleBGM");
	Audio::PlayWave("playBGM", 0.5f, true);
	isEnd = false;
	isAllEnd = false;

	camera->SetMatrixView({ 11.0f,12.5f,-10.0f }, { 11.0f,12.5f,0.0f }, { 0,1,0 });

	objectManager->Initialize();

	sceneCh->Initialize();
	sceneChangeLag = 0;

	playbg->Initialize();
	objectManager->Initialize();
	player->Initialize();
	boss->Initialize();

	isGameOver = false;
	isGameClear = false;
	CreateEnemies();
}

void Play::Update()
{
	lightGroup->SetAmbientColor(XMFLOAT3(coloramb));
	lightGroup->SetDirLightDir(0, { lightDir[0],lightDir[1],lightDir[2],1 });
	lightGroup->Update();
	objectManager->Update();
	player->Update();
	boss->Update();
	if(boss->IsDead())
		IsGameClear();
	
	collisionManager->CheckAllCollisions();

	//シーン切り替え
	if (sceneCh->GetToSmallEnd() &&
		!sceneCh->GetToBig())
	{
		if (isGameOver)
		{
			sceneChangeLag++;
			if (sceneChangeLag > 30)
			{
				next = GameOver;
				sceneCh->ChangeStart();
				isGameOver = false;
			}
		}
		if (isGameClear)
		{
			sceneChangeLag++;
			if (sceneChangeLag > 90)
			{
				next = GameClear;
				sceneCh->ChangeStart();
				isGameClear = false;
			}
		}
	}
	if (sceneCh->GetToBigEnd())
	{
		ShutDown();
	}
	sceneCh->Update();

	playbg->Update();

}

void Play::PreDraw()
{
#ifdef _DEBUG
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
#endif


	playbg->Draw();		

	//objectManager->PostDraw();

}

void Play::PostDraw()
{
	//plife->Draw();
	lectureTex[0]->CustomDraw(true);
	lectureTex[1]->CustomDraw(true);
	lectureTex[2]->CustomDraw(true);

	player->Draw();
	boss->Draw();
	sceneCh->Draw({ 0,0,0,1 });
	objectManager->PreDraw();

}

void Play::CreateStage()
{
	int map[15][60] = {
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,2,0,0,0,0},
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

void Play::CreateEnemies()
{
	Enemy* enemy = new Enemy({ 20,9,0 });
	objectManager->Add(enemy,false);
	Enemy* enemy2 = new Enemy({ 35,9,0 });
	objectManager->Add(enemy2, false);
	Enemy* enemy3 = new Enemy({ 20,14,0 });
	objectManager->Add(enemy3, false);
}
