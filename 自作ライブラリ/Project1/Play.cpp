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

DebugCamera* Play::camera = nullptr;

Play::Play()
{
	next = GameClear;
	//���C�g����
	lightGroup = LightGroup::Create();
	//3D�I�u�W�F�N�g�Ƀ��C�g���Z�b�g
	Object3D::SetLightGroup(lightGroup);
	//���C�g�F��ݒ�
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

	playbg = new PlayBackGround();
	playbg->SetPlayer(player);
}


Play::~Play()
{
	PtrDelete(lightGroup);

	objectManager->End();

	PtrDelete(sceneCh);
	PtrDelete(playbg);
}

void Play::Initialize()
{
	isEnd = false;
	isAllEnd = false;

	camera->SetMatrixView({ 11.0f,12.5f,-10.0f }, { 11.0f,12.5f,0.0f }, { 0,1,0 });

	objectManager->Initialize();

	sceneCh->Initialize();
	playbg->Initialize();
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
#endif // _DEBUG

	sceneCh->Update();

	playbg->Update();

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

	playbg->Draw();
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
