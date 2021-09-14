#include "Play.h"
#include"OBJLoader.h"
#include"PtrDelete.h"
#include"Object3D.h"
#include"Audio.h"
#include"ParticleEmitter.h"
#include "DrawMode.h"
#include"SphereCollider.h"
#include"imgui.h"
#include"TouchAbleObject.h"
#include"FBXManager.h"
#include"Grass.h"
#include "Input.h"

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
	//lightGroup->SetDirLightActive(1, false);
	//lightGroup->SetDirLightActive(2, false);
	//lightGroup->SetPointLightActive(0,false);
	//lightGroup->SetSpotLightActive(0, false);
	//lightGroup->SetCircleShadowActive(0, false);

	collisionManager = CollisionManager::GetInstance();
	objectManager = ObjectManager::GetInstance();
	TouchAbleObject *floor = new TouchAbleObject();
	floor->Initialize(OBJLoader::GetModel("floor"), { 0,-5,0 },{5,1,5});
	//floor->SetPosition({ 0,-5,0 });
	//floor->SetScale({ 5,1,5 });

	objectManager->Add(new Player());
	objectManager->Add(floor);
	//TouchAbleObject* chair = new TouchAbleObject();
	//chair->Initialize(OBJLoader::GetModel("chair"));
	//chair->SetPosition({ 0,-5,0 });
	//chair->SetScale({ 10,10,10 });
	//objectManager->Add(chair);

	for(int z = 0;z<5;z++)
	{
		for(int x = 0;x<5;x++)
		{
			TouchAbleObject* chair = new TouchAbleObject();
			chair->Initialize(OBJLoader::GetModel("chair"), { x * 20.0f - 42,-5,z * 20.0f - 42 }, { 3,3,3 });
			objectManager->Add(chair);
		}
	}
	shadowTex = new Sprite();
	
	//testObj = new GSShaderTestObj({0,10,0});
	//testNormalMapOBJ = new NormalMapTestOBJ();
	//testBloomOBJ = new Object();
	//testBloomOBJ->Create(OBJLoader::GetModel("sphere"));
	//	grass = new Grass();
	//grass->SetPosition({ 0,-5,0 });
	//testParticle = new ComputeShade(20);
	//for(int i = 0;i<20;i++)
	//{
	//	input[i].position = {};
	//	input[i].velocity = {i * 0.1f,0,0};
	//}
}


Play::~Play()
{
	PtrDelete(lightGroup);
	//PtrDelete(testNormalMapOBJ);
	//PtrDelete(testBloomOBJ);
	//PtrDelete(testObj);
	PtrDelete(shadowTex);

	objectManager->End();
	//PtrDelete(testFbx);
	//PtrDelete(grass);
	//PtrDelete(testParticle);
}

void Play::Initialize()
{
	isEnd = false;
}

void Play::Update()
{
	if(Input::TriggerKey(DIK_LSHIFT))
	{
		Input::ChangeMouseMove();
	}
	lightGroup->SetAmbientColor(XMFLOAT3(coloramb));
	lightGroup->SetDirLightDir(0, { lightDir[0],lightDir[1],lightDir[2],1 });
	lightGroup->Update();
	//if (DrawMode::GetMode() == DrawMode::NormalMap)
	//	testNormalMapOBJ->Update();
	//else
	//	testBloomOBJ->Update();
	//testObj->Update();
	objectManager->Update();

	//testParticle->Update(input);
	if(Input::TriggerKey(DIK_RETURN))
	{
		ParticleEmitter::CreateExplosion({ 0,0,0 });
	}
	collisionManager->CheckAllCollisions();
}

void Play::PreDraw()
{
	if (Object3D::GetDrawShadow())
	{
		bool preTarget = targetSphere;
		ImGui::Begin("Light");
		ImGui::SliderFloat3("LightDir", lightDir, -1.0f, 1.0f);
		ImGui::Text("NormalMap : 1");
		ImGui::Text("Bloom     : 2");
		ImGui::Text("Camera Rotation : RightMouseButton + Drag");
		ImGui::Text("Camera Zoom     : MouseWheel");
		ImGui::Text("Camera Reset    : R");
		ImGui::Checkbox("TargetSphere", &targetSphere);
		ImGui::End();
		if (targetSphere && !preTarget)
		{
			Object3D::GetCamera()->SetTarget(testObj->GetPosition());
		}
		else if (!targetSphere && preTarget)
		{
			Object3D::GetCamera()->SetTarget({ 0,0,0 });
		}
		Object3D::GetLightCamera()->SetLightDir({ lightDir[0],lightDir[1] ,lightDir[2] });
	}
	objectManager->PreDraw();
}

void Play::PostDraw()
{
	objectManager->PostDraw();
	//shadowTex->DrawSprite("shadowMap", { 0,0 }, 0, { 0.3f,0.3f }, { 1,1,1,1 }, { 0,0 });
}
