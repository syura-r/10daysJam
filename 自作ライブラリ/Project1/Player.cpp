#include"Player.h"

#include "Audio.h"
#include "BoxCollider.h"
#include "Collision.h"
#include"OBJLoader.h"
#include"SphereCollider.h"
#include"Input.h"
#include"DebugText.h"
#include"CollisionManager.h"
#include"CollisionAttribute.h"
#include "Easing.h"
#include"FBXManager.h"
#include "PtrDelete.h"
#include "ParticleEmitter.h"
#include "Play.h"
#define SIZE 1.0f

DebugCamera* Player::camera = nullptr;
LightCamera* Player::lightCamera = nullptr;

Player::Player()
{
	Create(FBXManager::GetModel("Hidari2"));
	naObject = new Object();
	naObject->Create(FBXManager::GetModel("Hidari1"));
	noObject = new Object();
	noObject->Create(FBXManager::GetModel("SwoedMode_2"));
	itiObject = new Object();
	itiObject->Create(FBXManager::GetModel("SwoedMode_1"));
	eObject = new Object();
	eObject->Create(FBXManager::GetModel("SwoedMode_3"));

	Initialize();

	BoxCollider* boxCollider = new BoxCollider({ 0,2.25f * scale.y,0,0 }, scale * 2);
	SetCollider(boxCollider);
	collider->SetAttribute(COLLISION_ATTR_ALLIES);
	collider->SetMove(true);

	itiObject->SetScale(scale);
	itiObject->SetColor(color);
	
	noObject->SetScale(scale);
	noObject->SetColor(color);

	eObject->SetScale(scale);
	eObject->SetColor(color);

	zanzoObject = new Object();
	zanzoObject->Create(FBXManager::GetModel("zan-zo"));
	zanzoObject->SetScale(scale);

	
	boomerangEffect = new Object();
	boomerangEffect->Create(FBXManager::GetModel("Boomerang"));
	boomerangEffect->SetScale(scale * 2.0f);

	//zanzoObject->SetColor({1,1,1,1});

	for(int i = 0;i<5;i++)
	{
		drawHidariGiri[i] = false;
		std::string modelName = "HidariGiri_0" + std::to_string(i + 1);
		hidariGiri[i] = new Object();
		hidariGiri[i]->Create(OBJLoader::GetModel(modelName));
		hidariGiri[i]->SetScale(scale * 3.5f);
	}
	drawFrame[0].SetTime(0, 0, 0, 148, 0, FbxTime::EMode::eFrames60);
	drawFrame[1].SetTime(0, 0, 0, 170, 0, FbxTime::EMode::eFrames60);
	drawFrame[2].SetTime(0, 0, 0, 184, 0, FbxTime::EMode::eFrames60);
	drawFrame[3].SetTime(0, 0, 0, 203, 0, FbxTime::EMode::eFrames60);
	drawFrame[4].SetTime(0, 0, 0, 220, 0, FbxTime::EMode::eFrames60);
	for (int i = 0; i < 4; i++)
	{
		hpSprites[i] = new Sprite();
	}

	downTex = new Sprite();
	upTex = new Sprite();
#ifdef _DEBUG
	hitObj = new Object();
	hitObj->Create(OBJLoader::GetModel("box"));
	hitObj->SetScale(scale * 4);
	//hitObj->SetScale({ 0.8f,0.8f,0.8f });
	hitObj->SetColor({ 1,1,1,0.4f });
#endif _DEBUG
}

Player::~Player()
{
	PtrDelete(downTex);
	PtrDelete(upTex);
	PtrDelete(itiObject);
	PtrDelete(noObject);
	PtrDelete(eObject);
	PtrDelete(naObject);
	PtrDelete(zanzoObject);
	PtrDelete(boomerangEffect);

	for(int i = 0;i<5;i++)
	{
		PtrDelete(hidariGiri[i]);
	}
	for (int i = 0; i < 4; i++)
	{
		PtrDelete(hpSprites[i]);
	}

#ifdef _DEBUG
	PtrDelete(hitObj);
#endif _DEBUG

}

void Player::Initialize()
{
	name = typeid(*this).name();
	//scale = { 0.3f,1,0.5f };
	position = { 3,10,0 };
	//rotation = { 90,0,0 };
	scale = { 0.2f, 0.2f, 0.2f };
	color = { 0,0,0,1 };
	naPos = position;
	naObject->SetPosition(naPos);
	//naObject->SetRotation(rotation);
	naObject->SetScale(scale);
	naObject->SetColor(color);

	prePos = position;
	nowAttackState = NoAttack;
	lightCamera->SetTarget(position + Vector3{ 0,1,0 });
	camera->SetTarget(Vector3{ 11, position.y + 2, 0 });
	moveRight = true;
	moveShift = false;

	nowAnimationState = Wait;

	meleeAttackStage = 1;
	attackFrag = false;
	attackCounter = 0;

	boomerangCounter = 0;
	boomerangSpeed = BoomerangMaxSpeed;

	continueMeleeAttack = false;
	damage = false;
	hp = 3;

	drawZanzoFrag = false;
	boomerang = false;

	notMove = false;
	startFight = false;
	texSizeY = 0;
	texSizeCounter = 0;

	drawBlackTex = false;
	waitFight = false;
	end = false;
	endCounter = 0;
	finish = false;
	cameramove = true;

	rotation.y = 0;
	
	naObject->SetRotation(rotation);
	itiObject->SetRotation(rotation);
	noObject->SetRotation(rotation);
	eObject->SetRotation(rotation);

	itiObject->SetPosition(position);
	noObject->SetPosition(position);
	eObject->SetPosition(position);
	itiObject->SetColor(color);
	noObject->SetColor(color);
	eObject->SetColor(color);

	itiObject->Update();
	noObject->Update();
	eObject->Update();
}

void Player::Update()
{
	
	Move();
	Attack();
	if(end)
	{
		EndMovie();
		return;
	}
//-----------------------------�_���[�W����-----------------------------------------
	if(damage)
	{
		if(invCounter >= 120)
		{
			damage = false;
			color = { 0,0,0,1 };
			naObject->SetColor(color);
			noObject->SetColor(color);
			itiObject->SetColor(color);
			eObject->SetColor(color);
		}
		else
		{
			if(invCounter % 10 == 0)
			{
				if (color.w == 1)
					color.w = 0;
				else
					color.w = 1;
				naObject->SetColor(color);
				noObject->SetColor(color);
				itiObject->SetColor(color);
				eObject->SetColor(color);
			}
		}
		invCounter++;
	}
	
//----------------------------------------------------------------------------------
	
	//��������
	if (!onGround)
	{
		//����������
		const float fallAcc = -0.02f;
		const float fallVYMin = -0.5f;
		//����
		fallV.m128_f32[1] = max(fallV.m128_f32[1] + fallAcc, fallVYMin);
		//�ړ�
		position.x += fallV.m128_f32[0];
		position.y += fallV.m128_f32[1];
		position.z += fallV.m128_f32[2];
	}
	//�W�����v����(�n�ʍ��܂ŃR�����g�A�E�g)
	else if ((Input::TriggerKey(DIK_SPACE) || Input::TriggerPadButton(XINPUT_GAMEPAD_A)) && !notMove && nowAttackState != MeleeAttack)
	{
		onGround = false;
		const float jumpVYFist = 0.5f;//�W�����v�����������
		fallV = { 0,jumpVYFist,0,0 };
		Audio::PlayWave("jump",0.5f);
		//�����]���A�j���[�V�������ȊO�̓A�j���[�V�������Đ�
		if (nowAnimationState != Turn && nowAttackState != Boomerang)
		{
			//�V�����v�A�j���[�V�����񏉊�������
			FBXManager::GetModel("Hidari2")->SetAnimationFrame(121, 180, 2);
			FBXManager::GetModel("Hidari1")->SetAnimationFrame(121, 180, 2);
			nowAnimationState = Jump;
			if (moveRight)
			{
				rotation.y = 0;
				naObject->SetRotation(rotation);
			}
		}
	}
//----------------------------------------�}�b�v�Ƃ̔���E�r�ˏ���-------------------------------------------
	//�R���C�_�[�X�V
	Object::Update();
	//�R���C�_�[���擾
	BoxCollider* boxCollider = dynamic_cast<BoxCollider*>(collider);
	assert(boxCollider);

	//���̏�[���狅�̉��[�܂ł̃��C�L���X�g�p���C������
	Ray ray;
	ray.start = boxCollider->center;
	ray.start.m128_f32[1] += boxCollider->GetScale().y;
	ray.dir = { 0,-1,0,0 };
	RaycastHit raycastHit;

	//�ڒn���
	if (onGround)
	{
		//�X���[�Y�ɍ������ׂ̋z������
		const float absDistance = 0.2f;
		//�ڒn���ێ�
		if (CollisionManager::GetInstance()->Raycast(collider, ray, COLLISION_ATTR_LANDSHAPE,
			&raycastHit, boxCollider->GetScale().y * 2.0f + absDistance))
		{
			onGround = true;
			position.y -= raycastHit.distance - boxCollider->GetScale().y * 2.0f;
			//�s��X�V�Ȃ�
			Object::Update();
		}
		//�n�ʂ��Ȃ��̂ŗ���
		else {
			onGround = false;
			fallV = {};
		}
	}
	//�������
	else if (fallV.m128_f32[1] <= 0.0f)
		if (CollisionManager::GetInstance()->Raycast(collider, ray, COLLISION_ATTR_LANDSHAPE,
			&raycastHit, boxCollider->GetScale().y * 2.0f))
		{
			//���n
			onGround = true;
			position.y -= raycastHit.distance - boxCollider->GetScale().y * 2.0f;
			//�s��X�V�Ȃ�
			Object::Update();
		}

	

	//�N�G���[�R�[���o�b�N�̊֐��I�u�W�F�N�g
	PlayerQueryCallBack callback(boxCollider);
	//���ƒn�`�̌�����S����
	CollisionManager::GetInstance()->QueryBox(*boxCollider, &callback, COLLISION_ATTR_LANDSHAPE);

	if (callback.move.m128_f32[0] < -speed)
		callback.move.m128_f32[0] = -speed;
	else if (callback.move.m128_f32[0] > speed)
		callback.move.m128_f32[0] = speed;
	//�����ɂ��r�˕��𓮂���
	position.x += callback.move.m128_f32[0];
	position.y += callback.move.m128_f32[1];
	position.z += callback.move.m128_f32[2];
	if (callback.move.m128_f32[1] < -0.1f)
		fallV.m128_f32[1] = 0;

	//�r�˂Œn�ʂɗ����Ă�ꍇ
	if (!onGround && prePos.y == position.y && callback.move.m128_f32[1] != 0)
		onGround = true;
	//�R���C�_�[�X�V	
	Object::Update();

//----------------------------------------------------------------------------------------------------------------
	
	if (prePos != position && !startFight && cameramove)//�J�����̃^�[�Q�b�g�X�V
	{
		Vector3 targetPos = Vector3{position.x,12.5f,0};
		if(position.x < 11)
		{
			targetPos.x = 11;
		}
		lightCamera->SetTarget(targetPos);
		camera->SetTarget(targetPos);
	}

	if (!boomerang)
	{
		naPos = position;
	}
	else
	{
		boomerangEffect->SetPosition(naPos + Vector3{ 0, 2.25f * scale.y + 0.3f, 0});
		boomerangEffect->Update();
	}
	if(drawZanzoFrag == true)
	{
		zanzoObject->SetRotation(rotation);
		zanzoObject->SetPosition(position);
		zanzoObject->Update();
	}

	if (nowAttackState == ULT)
	{
		for (int i = 0; i < 5; i++)
		{
			hidariGiri[i]->SetRotation(rotation);
			float offset = 0.6f;
			if (!moveRight)
				offset *= -1;
			hidariGiri[i]->SetPosition(position + Vector3{ offset, 2.25f * scale.y, 0.02f - i*0.01f});
			hidariGiri[i]->Update();
		}
	}
	if(nowAttackState == MeleeAttack || nowAttackState == JumpAttack || nowAttackState == ULT)
	{
		itiObject->SetRotation(rotation);
		noObject-> SetRotation(rotation);
		eObject->  SetRotation(rotation);
		itiObject->SetPosition(position);
		noObject ->SetPosition(position);
		eObject  ->SetPosition(position);
		itiObject->Update();
		noObject ->Update();
		eObject  ->Update();
	}

	naObject->SetPosition(naPos);

	//�R���C�_�[�X�V	
	Object::Update();
	naObject->Update();



#ifdef _DEBUG
	rejectVal.x = callback.move.m128_f32[0];
	rejectVal.y = callback.move.m128_f32[1];
	rejectVal.z = callback.move.m128_f32[2];

	if (rejectVal.y != 0)
	{
		printf("rejectVal.y : %f\n", rejectVal.y);
	}
	if (rejectVal.x != 0)
	{
		printf("rejectVal.x : %f\n", rejectVal.x);
	}

	float offsetX = 0.6f;
	if (!moveRight)
		offsetX *= -1;
	if (nowAttackState == Boomerang)
	{
		hitObj->SetScale({ scale.x * 4,scale.y * 2,scale.z * 4 });
		hitObj->SetPosition(position + Vector3{ 0,2.25f * scale.y / 2,0});
	}
	else
	{
		hitObj->SetScale(scale * 4);
		hitObj->SetPosition(position + Vector3{ 0,2.25f * scale.y,0 });
	}
	//hitObj->SetPosition(position + Vector3{ offsetX,2.25f * scale.y,0 });

	hitObj->Update();
#endif _DEBUG
}

void Player::OnCollision(const CollisionInfo & info)
{
	if(damage || nowAttackState == JumpAttack || info.collider->GetAttribute() != COLLISION_ATTR_ENEMYS )
		return;

	color = { 0.4f,0,0,1 };
	if (startFight)
	{
		hp--;
		if (hp <= 0)
		{
			dead = true;

			ParticleEmitter::CreateSparkEffects(position + Vector3{ 0.0f,1.0f,-0.6f }, { 1,0,0 });
			Play::IsGameOver();
		}
	}
	damage = true;
	invCounter = 0;
	Audio::PlayWave("hit1", 0.3f);
}
void Player::EndFight()
{
	boomerang = false;
	attackFrag = false;
	endCounter = 0;
	startFight = false;
	notMove = true;
	cameramove = false;
	color = { 0,0,0,1 };
	naObject->SetColor(color);
	nowAnimationState = Wait;
	FBXManager::GetModel("Hidari2")->SetAnimationFrame(0, 60, 2);
	FBXManager::GetModel("Hidari1")->SetAnimationFrame(0, 60, 2);
	nowAttackState = JumpAttack;
	FBXManager::GetModel("SwoedMode_1")->SetAnimationFrame(0, 30);
	FBXManager::GetModel("SwoedMode_2")->SetAnimationFrame(0, 30);
	FBXManager::GetModel("SwoedMode_3")->SetAnimationFrame(0, 30);
	Audio::PlayWave("slow", 0.3f);
	itiObject->SetColor(color);
	noObject->SetColor(color);
	eObject->SetColor(color);

}

void Player::Draw()
{
#ifdef _DEBUG
	ImGui::Begin("PlayerState");
	ImGui::Text("onGround : %d\n", onGround);
	ImGui::Text("position.x : %f\n", position.x);
	ImGui::Text("position.y : %f\n", position.y);
	ImGui::Text("fallVelY : %f\n", fallV.m128_f32[1]); 
	ImGui::Text("rejectVal.y : %f\n", rejectVal.y);

	ImGui::End();
#endif
	if (startFight)
	{
		for (int i = 0; i < 4; i++)
		{
			std::string texName = "HPber_0" + std::to_string(i);
			if (hp >= i)
				hpSprites[i]->DrawSprite(texName, { 100,100 }, 0, { 0.5f,0.5f }, { 1,1,1,1 }, { 0,0 });
		}
	}
	DirectXLib::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if(nowAnimationState == Attacks && nowAttackState != Boomerang)
	{
		itiObject->CustomDraw(true);
		noObject->CustomDraw(true);
		eObject->CustomDraw(true);
		if (drawZanzoFrag == true)
			zanzoObject->CustomDraw(true);
		if(nowAttackState == ULT)
		{
			for (int i = 0; i < 5; i++)
			{
				if (drawHidariGiri[i])
					hidariGiri[i]->CustomDraw(false,false);
			}
		}
		return;
	}
	CustomDraw(true);
	naObject->CustomDraw(true);
	if (boomerang)
		boomerangEffect->CustomDraw(true);

	if(drawBlackTex)
	{
		upTex->DrawSprite("white1x1",{960,0},0,{1920,texSizeY},{0,0,0,1},{0.5f,0});
		downTex->DrawSprite("white1x1", { 960,1080 }, 0, { 1920,texSizeY }, { 0,0,0,1 }, { 0.5f,1 });

	}
#ifdef _DEBUG
	//if (nowAttackState == JumpAttack)
		//hitObj->Draw();
#endif _DEBUG

}

void Player::Attack()
{
//-----------------------------------------�U����������---------------------------------------------
	if (!attackFrag)
	{
		if (nowAttackState == NoAttack && (Input::TriggerKey(DIK_RETURN) || Input::TriggerPadButton(XINPUT_GAMEPAD_B)) && !notMove)
		{
			//----------------------------�ʏ�U���̔�������----------------------------------------
			if (onGround)
			{
				nowAttackState = MeleeAttack;
				nowAnimationState = Attacks;
				meleeAttackStage = 1;
				attackFrag = true;
				attackCounter = 0;
				continueMeleeAttack = false;
				FBXManager::GetModel("SwoedMode_1")->SetAnimationFrame(0, 30);
				FBXManager::GetModel("SwoedMode_2")->SetAnimationFrame(0, 30);
				FBXManager::GetModel("SwoedMode_3")->SetAnimationFrame(0, 30);
				FBXManager::GetModel("zan-zo")->SetAnimationFrame(11, 30);
				
				if (!moveRight)
					rotation.y = 180;
				else
					rotation.y = 0;
				Audio::PlayWave("sword1",0.5f);
			}//-------------------------------------------------------------------------------------
			//----------------------------�W�����v�U���̔�������------------------------------------
			else
			{
				nowAttackState = JumpAttack;
				nowAnimationState = Attacks;
				attackFrag = true;
				FBXManager::GetModel("SwoedMode_1")->SetAnimationFrame(101, 130);
				FBXManager::GetModel("SwoedMode_2")->SetAnimationFrame(101, 130);
				FBXManager::GetModel("SwoedMode_3")->SetAnimationFrame(101, 130);
				FBXManager::GetModel("zan-zo")->SetAnimationFrame(105, 130);
				Audio::PlayWave("sword2",0.5f);

			}//-------------------------------------------------------------------------------------
		}
		
		//----------------------------�u�[�������̔�������------------------------------------------
		if ((Input::TriggerKey(DIK_Z) || Input::TriggerPadButton(XINPUT_GAMEPAD_X)) && !notMove)
		{
			nowAttackState = Boomerang;
			nowAnimationState = Attacks;
			attackFrag = true;
			boomerangCounter = 0;
			if (moveRight)
			{
				boomerangMoveRight = true;
				boomerangSpeed = BoomerangMaxSpeed;
			}
			else
			{
				boomerangMoveRight = false;
				boomerangSpeed = -BoomerangMaxSpeed;
			}
			if (!moveRight)
				rotation.y = 180;
			else
				rotation.y = 0;
			boomerang = false;
			FBXManager::GetModel("Hidari2")->SetAnimationFrame(181, 210);
			FBXManager::GetModel("Hidari1")->SetAnimationFrame(181, 210);
		}//-------------------------------------------------------------------------------------

		//-------------------------------���a�蔭������---------------------------------------------
		//if(Input::TriggerKey(DIK_X))
		//{
		//	nowAttackState = ULT;
		//	nowAnimationState = Attacks;
		//	attackFrag = true;
		//	FBXManager::GetModel("SwoedMode_1")->SetAnimationFrame(131, 240);
		//	FBXManager::GetModel("SwoedMode_2")->SetAnimationFrame(131, 240);
		//	FBXManager::GetModel("SwoedMode_3")->SetAnimationFrame(131, 240);

		//}

		//------------------------------------------------------------------------------------------
	}
//----------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------�U���X�V����------------------------------------------------------------
	else
	{
		switch (nowAttackState)
		{
		case MeleeAttack://---------------------------------�ʏ�U��----------------------------------------------------------
		{
			auto nowAnimationTime = FBXManager::GetModel("SwoedMode_1")->GetCurrentAnimationTime();
			FbxTime beginTime;
			FbxTime endTime;
			drawZanzoFrag = false;
			switch (meleeAttackStage)
			{
			case 1://-----------------------------------------1�i�K��----------------------------------------------------------
			{
				beginTime.SetTime(0, 0, 0, 12, 0, FbxTime::EMode::eFrames60);
				endTime.SetTime(0, 0, 0, 18, 0, FbxTime::EMode::eFrames60);
				if (nowAnimationTime >= beginTime && nowAnimationTime <= endTime)
				{
					float offsetX = 0.6f;
					if (!moveRight)
						offsetX *= -1;
					BoxCollider* boxCollider = new BoxCollider({ offsetX,2.25f * scale.y,0,0 }, scale * 2);
					boxCollider->SetAttribute(COLLISION_ATTR_ATTACK);
					boxCollider->SetObject(this);
					boxCollider->Update();
					//�N�G���[�R�[���o�b�N�̊֐��I�u�W�F�N�g
					PlayerQueryCallBack callback(boxCollider);
					CollisionManager::GetInstance()->QueryBox(*boxCollider, &callback, COLLISION_ATTR_ENEMYS,COLLISION_ATTR_BOSS, boxCollider);
					drawZanzoFrag = true;
					FBXManager::GetModel("zan-zo")->PlayAnimation();

				}
				if( Input::TriggerKey(DIK_RETURN) || Input::TriggerPadButton(XINPUT_GAMEPAD_B))
				{
					continueMeleeAttack = true;
				}
				FBXManager::GetModel("SwoedMode_2")->PlayAnimation();
				FBXManager::GetModel("SwoedMode_3")->PlayAnimation();
				if (!FBXManager::GetModel("SwoedMode_1")->PlayAnimation())
				{
					if (continueMeleeAttack)
					{
						continueMeleeAttack = false;
						FBXManager::GetModel("SwoedMode_1")->SetAnimationFrame(31, 60);
						FBXManager::GetModel("SwoedMode_2")->SetAnimationFrame(31, 60);
						FBXManager::GetModel("SwoedMode_3")->SetAnimationFrame(31, 60);
						FBXManager::GetModel("zan-zo")->SetAnimationFrame(43, 60);
						meleeAttackStage = 2;
						Audio::PlayWave("sword1",0.5f);
					}
					else
					{
						drawZanzoFrag = false;
						attackFrag = false;
						nowAttackState = NoAttack;
						nowAnimationState = Wait;
						FBXManager::GetModel("Hidari1")->SetAnimationFrame(221, 300);
						FBXManager::GetModel("Hidari2")->SetAnimationFrame(221, 300);
					}
				}
				break;
			}//---------------------------------------------------------------------------------------------------------------
			case 2://----------------------------------------2�i�K��----------------------------------------------------------
			{
				beginTime.SetTime(0, 0, 0, 44, 0, FbxTime::EMode::eFrames60);
				endTime.SetTime(0, 0, 0, 49, 0, FbxTime::EMode::eFrames60);

				if (nowAnimationTime >= beginTime && nowAnimationTime <= endTime)
				{
					float offsetX = 0.6f;
					if (!moveRight)
						offsetX *= -1;
					BoxCollider* boxCollider = new BoxCollider({ offsetX,2.25f * scale.y,0,0 }, scale * 2);
					boxCollider->SetAttribute(COLLISION_ATTR_ATTACK);
					boxCollider->SetObject(this);
					boxCollider->Update();
					//�N�G���[�R�[���o�b�N�̊֐��I�u�W�F�N�g
					PlayerQueryCallBack callback(boxCollider);
					CollisionManager::GetInstance()->QueryBox(*boxCollider, &callback, COLLISION_ATTR_ENEMYS, COLLISION_ATTR_BOSS, boxCollider);
					if (nowAnimationTime >= beginTime && nowAnimationTime < endTime)
					{
						drawZanzoFrag = true;
						FBXManager::GetModel("zan-zo")->PlayAnimation();
					}
				}
				else if ( Input::TriggerKey(DIK_RETURN) || Input::TriggerPadButton(XINPUT_GAMEPAD_B))
				{
					continueMeleeAttack = true;
				}
				FBXManager::GetModel("SwoedMode_2")->PlayAnimation();
				FBXManager::GetModel("SwoedMode_3")->PlayAnimation();
				if (!FBXManager::GetModel("SwoedMode_1")->PlayAnimation())
				{
					if (continueMeleeAttack)
					{
						continueMeleeAttack = false;
						FBXManager::GetModel("SwoedMode_1")->SetAnimationFrame(61, 100);
						FBXManager::GetModel("SwoedMode_2")->SetAnimationFrame(61, 100);
						FBXManager::GetModel("SwoedMode_3")->SetAnimationFrame(61, 100);
						FBXManager::GetModel("zan-zo")->SetAnimationFrame(70, 100);
						meleeAttackStage = 3;
						Audio::PlayWave("sword2",0.5f);
					}
					else
					{
						drawZanzoFrag = false;
						attackFrag = false;
						nowAttackState = NoAttack;
						nowAnimationState = Wait;
						FBXManager::GetModel("Hidari1")->SetAnimationFrame(221, 300);
						FBXManager::GetModel("Hidari2")->SetAnimationFrame(221, 300);
					}
				}

				break;
			}//---------------------------------------------------------------------------------------------------------------
			case 3://----------------------------------------3�i�K��----------------------------------------------------------
			{
				beginTime.SetTime(0, 0, 0, 70, 0, FbxTime::EMode::eFrames60);
				endTime.SetTime(0, 0, 0, 79, 0, FbxTime::EMode::eFrames60);
				
				if (nowAnimationTime >= beginTime && nowAnimationTime <= endTime)
				{
					float offsetX = 0.6f;
					if (!moveRight)
						offsetX *= -1;
					BoxCollider* boxCollider = new BoxCollider({ offsetX,2.25f * scale.y,0,0 }, scale * 2);
					boxCollider->SetAttribute(COLLISION_ATTR_ATTACK);
					boxCollider->SetObject(this);
					boxCollider->Update();
					//�N�G���[�R�[���o�b�N�̊֐��I�u�W�F�N�g
					PlayerQueryCallBack callback(boxCollider);
					CollisionManager::GetInstance()->QueryBox(*boxCollider, &callback, COLLISION_ATTR_ENEMYS, COLLISION_ATTR_BOSS, boxCollider);
					drawZanzoFrag = true;
					FBXManager::GetModel("zan-zo")->PlayAnimation();
				}
				FBXManager::GetModel("SwoedMode_2")->PlayAnimation();
				FBXManager::GetModel("SwoedMode_3")->PlayAnimation();
				if (!FBXManager::GetModel("SwoedMode_1")->PlayAnimation())
				{
					drawZanzoFrag = false;
					attackFrag = false;
					nowAttackState = NoAttack;
					nowAnimationState = Wait;
					FBXManager::GetModel("Hidari1")->SetAnimationFrame(221, 300);
					FBXManager::GetModel("Hidari2")->SetAnimationFrame(221, 300);
				}

				break;
			}//---------------------------------------------------------------------------------------------------------------
			default:
				break;
			}	
			break;
		}//----------------------------------------------------------------------------------------------------------------------
		case Boomerang://---------------------------------------�u�[������---------------------------------------------------------
		{
			SphereCollider* sphereCollider = new SphereCollider({ 0,2.25f * scale.y,0,0 }, 0.5f);
			sphereCollider->SetAttribute(COLLISION_ATTR_ATTACK);
			sphereCollider->SetObject(naObject);
			sphereCollider->Update();
			//�N�G���[�R�[���o�b�N�̊֐��I�u�W�F�N�g
			PlayerQueryCallBack callback(sphereCollider);
			CollisionManager::GetInstance()->QuerySphere(*sphereCollider, &callback, COLLISION_ATTR_ENEMYS, COLLISION_ATTR_BOSS, sphereCollider);

			FbxTime beginTime;
			beginTime.SetTime(0, 0, 0, 200, 0, FbxTime::EMode::eFrames60);

			FBXManager::GetModel("Hidari2")->PlayAnimation();
			FBXManager::GetModel("Hidari1")->PlayAnimation(true);
			FBXManager::GetModel("Boomerang")->PlayAnimation(true);
			auto nowAnimationTime = FBXManager::GetModel("Hidari1")->GetCurrentAnimationTime();
			if (nowAnimationTime == beginTime)
			{
				FBXManager::GetModel("Boomerang")->SetAnimationFrame(0, 30);
				FBXManager::GetModel("Hidari1")->SetAnimationFrame(200, 210);
				boomerang = true;
				boomerangEffect->SetRotation(rotation);
				Audio::PlayWave("sword2",0.5f);
			}
			if (boomerang && boomerangCounter >= BoomerangTime * 4)
			{
				boomerang = false;
				attackFrag = false;
				nowAttackState = NoAttack;
				nowAnimationState = Wait;
				FBXManager::GetModel("Hidari1")->SetAnimationFrame(221, 300);
				FBXManager::GetModel("Hidari2")->SetAnimationFrame(221, 300);
				naObject->SetRotation(rotation);
			}
			break;
		}//----------------------------------------------------------------------------------------------------------------------
		case JumpAttack://---------------------------------------�W�����v�U��----------------------------------------------------
		{
			auto nowAnimationTime = FBXManager::GetModel("SwoedMode_3")->GetCurrentAnimationTime();
			FbxTime beginTime;
			FbxTime endTime;
			beginTime.SetTime(0, 0, 0, 105, 0, FbxTime::EMode::eFrames60);
			endTime.SetTime(0, 0, 0, 130, 0, FbxTime::EMode::eFrames60);

			if (nowAnimationTime >= beginTime && nowAnimationTime <= endTime)
			{

				SphereCollider* sphereCollider = new SphereCollider({ 0,2.25f * scale.y,0,0 }, 0.8f);
				sphereCollider->SetAttribute(COLLISION_ATTR_ATTACK);
				sphereCollider->SetObject(this);
				sphereCollider->Update();
				//�N�G���[�R�[���o�b�N�̊֐��I�u�W�F�N�g
				PlayerQueryCallBack callback(sphereCollider);
				CollisionManager::GetInstance()->QuerySphere(*sphereCollider, &callback, COLLISION_ATTR_ENEMYS, COLLISION_ATTR_BOSS, sphereCollider);
				drawZanzoFrag = true;
				FBXManager::GetModel("zan-zo")->PlayAnimation();
			}
			FBXManager::GetModel("SwoedMode_1")->PlayAnimation();
			FBXManager::GetModel("SwoedMode_2")->PlayAnimation();

			if(!FBXManager::GetModel("SwoedMode_3")->PlayAnimation())
			{
				drawZanzoFrag = false;
				attackFrag = false;
				nowAttackState = NoAttack;
				nowAnimationState = Wait;
				FBXManager::GetModel("Hidari1")->SetAnimationFrame(221, 300);
				FBXManager::GetModel("Hidari2")->SetAnimationFrame(221, 300);
				if (moveRight)
					rotation.y = 0;
				else
					rotation.y = 180;
				naObject->SetRotation(rotation);
			}
			break;
		}//----------------------------------------------------------------------------------------------------------------------
		case ULT://-------------------------------------------------���a��-------------------------------------------------------
		{
			auto nowAnimationTime = FBXManager::GetModel("SwoedMode_3")->GetCurrentAnimationTime();

			for (int i = 0; i < 5; i++)
			{
				hidariGiri[i]->SetRotation(rotation);
				float offset = 0.7f;
				if (!moveRight)
					offset *= -1;
				hidariGiri[i]->SetPosition(position + Vector3{ offset, 2.25f * scale.y, 0.02f - i * 0.01f });
				hidariGiri[i]->Update();

				if (nowAnimationTime == drawFrame[i])
				{
					drawHidariGiri[i] = true;
					Audio::PlayWave("hit2", 0.3f);
				}
			}
			FBXManager::GetModel("SwoedMode_1")->PlayAnimation();
			FBXManager::GetModel("SwoedMode_2")->PlayAnimation();

			if (!FBXManager::GetModel("SwoedMode_3")->PlayAnimation())
			{
				if (endCounter == 11)
				{
					attackFrag = false;
					nowAnimationState = Wait;
					velocity = (Vector3{ 55, 8.45f, 0 } - position) / 30;
					endCounter++;
					Audio::PlayWave("finish", 0.3f);
				}
			}

			break;
		}//----------------------------------------------------------------------------------------------------------------------
		default:
			break;
		}
	}
//------------------------------------------------------------------------------------------------------------------------------------
}

void Player::Move()
{
	prePos = position;
//-----------------------------------------�ړ�����---------------------------------------------
	if (nowAttackState != MeleeAttack && (Input::DownKey(DIK_D) || Input::DownKey(DIK_A) || Input::CheckPadLStickRight() || Input::CheckPadLStickLeft() )&& !notMove)
	{
		if (Input::DownKey(DIK_D)|| Input::CheckPadLStickRight())
		{
			//��]�����֘A�̏�����
			if (!moveRight)
			{
				moveRight = true;
				if (nowAttackState != JumpAttack)
				{
					rotation.y = 180;
					nowAnimationState = Turn;
					if (nowAttackState != Boomerang)
					{
						FBXManager::GetModel("Hidari2")->SetAnimationFrame(61, 120, 3);
						FBXManager::GetModel("Hidari1")->SetAnimationFrame(61, 120, 3);
					}
				}
				naObject->SetRotation(rotation);

			}
			position.x += speed;
			if (nowAttackState == Boomerang)
				naPos.x += speed;
		}
		if (Input::DownKey(DIK_A) || Input::CheckPadLStickLeft())
		{
			//��]�����֘A�̏�����
			if (moveRight)
			{
				moveRight = false;
				if (nowAttackState != JumpAttack)
				{
					rotation.y = 0;
					nowAnimationState = Turn;
					if (nowAttackState != Boomerang)
					{
						FBXManager::GetModel("Hidari2")->SetAnimationFrame(61, 120, 3);
						FBXManager::GetModel("Hidari1")->SetAnimationFrame(61, 120, 3);
					}
				}
				naObject->SetRotation(rotation);
			}
			position.x -= speed;
			if (nowAttackState == Boomerang)
				naPos.x -= speed;
		}

		if (position.x < 1)
			position.x = 1;
		if(startFight)
		{
			if (position.x < WallLeft)
				position.x = WallLeft;
			else if (position.x > WallRight)
				position.x = WallRight;
		}
		else if (position.x >= WallLeft)
		{
			drawBlackTex = true;
			notMove = true;
			nowAnimationState = Walk;
			FBXManager::GetModel("Hidari2")->SetAnimationFrame(221, 300);
			FBXManager::GetModel("Hidari1")->SetAnimationFrame(221, 300);
			return;
		}

		//�ҋ@�A�j���[�V�����̎��͕��s�A�j���[�V���������s
		if (nowAnimationState == Wait)
		{
			FBXManager::GetModel("Hidari2")->SetAnimationFrame(0, 60, 2);
			FBXManager::GetModel("Hidari1")->SetAnimationFrame(0, 60, 2);
			nowAnimationState = Walk;
		}
	}
	else if(nowAnimationState == Walk && !notMove)
	{
		nowAnimationState = Wait;
		FBXManager::GetModel("Hidari2")->SetAnimationFrame(221, 300);
		FBXManager::GetModel("Hidari1")->SetAnimationFrame(221, 300);
	}
	if (nowAnimationState == Wait || nowAnimationState == Walk)
	{
		FBXManager::GetModel("Hidari2")->PlayAnimation(true);
		FBXManager::GetModel("Hidari1")->PlayAnimation(true);
	}

//---------------------------------------------------------------------------------------------------

//------------------------------------�����]�����̉�]����---------------------------------------

	if (nowAnimationState == Turn )
	{
		if (nowAttackState != Boomerang)
		{
			FBXManager::GetModel("Hidari2")->PlayAnimation();
			if (!FBXManager::GetModel("Hidari1")->PlayAnimation())
			{
				nowAnimationState = Wait;
				if (!moveRight)
					rotation.y = 180;
				else
					rotation.y = 0;
				naObject->SetRotation(rotation);

			}
		}
		else
		{
			if (!moveRight)
				rotation.y = 180;
			else
				rotation.y = 0;
		}
	}
//-----------------------------------------------------------------------------------------------

//-----------------------------�W�����v�A�j���[�V�����̍Đ�����----------------------------------

	if (nowAnimationState == Jump)
	{
		FBXManager::GetModel("Hidari2")->PlayAnimation();
		if (!FBXManager::GetModel("Hidari1")->PlayAnimation())
		{
			nowAnimationState = Wait;
		}
	}

//------------------------------------------------------------------------------------------------


//---------------------------------�u�[�������̈ړ�����--------------------------------------------
	if (boomerang)
	{
		naPos.x += boomerangSpeed;
		if (boomerangCounter >= BoomerangTime)
		{
			if (boomerangMoveRight && boomerangSpeed > -BoomerangMaxSpeed)
			{
				boomerangSpeed -= BoomerangMaxSpeed / BoomerangTime;
			}
			else if (!boomerangMoveRight && boomerangSpeed < BoomerangMaxSpeed)
			{
				boomerangSpeed += BoomerangMaxSpeed / BoomerangTime;
			}
			naPos.y += (position.y - naPos.y) / (BoomerangTime * 4 - boomerangCounter);
		}
		boomerangCounter++;
	}
//------------------------------------------------------------------------------------------------

//--------------------------------�{�X��O�̎������o----------------------------------------------
	if (notMove && !startFight && !waitFight)
	{
		if (position.x >= WallLeft + (WallRight - WallLeft) / 2)
		{
			//notMove = false;
			nowAnimationState = Wait;
			FBXManager::GetModel("Hidari2")->SetAnimationFrame(221, 300);
			FBXManager::GetModel("Hidari1")->SetAnimationFrame(221, 300);
			waitFight = true;
			lightCamera->SetTarget({ WallLeft + (WallRight - WallLeft) / 2 ,12.5f,0 });
			camera->SetTarget({ WallLeft + (WallRight - WallLeft) / 2 ,12.5f,0 });
		}
		else
			position.x += speed;
	}

	if (drawBlackTex)
	{
		if (notMove)
		{
			if (texSizeCounter < 30)
				texSizeCounter++;
		}
		else if(startFight)
		{
			if (texSizeCounter > 0)
				texSizeCounter--;
			else
				drawBlackTex = false;
		}
		texSizeY = Easing::EaseOutBack(0, 100, 30, texSizeCounter);
	}

//------------------------------------------------------------------------------------------------
}

void Player::EndMovie()
{
	itiObject->SetRotation(rotation);
	noObject->SetRotation(rotation);
	eObject->SetRotation(rotation);
	itiObject->SetPosition(position);
	noObject->SetPosition(position);
	eObject->SetPosition(position);
	itiObject->Update();
	noObject->Update();
	eObject->Update();

	if (endCounter == 0&&(position.x > 49 + 0.1f || position.x < 49 - 0.1f))
	{
		position += velocity;
		naObject->SetPosition(position);

		//�R���C�_�[�X�V	
		Object::Update();
		naObject->Update();

		return;
	}
	
	if (endCounter == 10)
	{
		nowAttackState = ULT;
		nowAnimationState = Attacks;
		attackFrag = true;
		FBXManager::GetModel("SwoedMode_1")->SetAnimationFrame(131, 240);
		FBXManager::GetModel("SwoedMode_2")->SetAnimationFrame(131, 240);
		FBXManager::GetModel("SwoedMode_3")->SetAnimationFrame(131, 240);
		endCounter = 11;
	}
	if (endCounter < 11)
		endCounter++;

	if (endCounter >= 12)
	{
		if (position.x <= 55 + 0.01f && position.x >= 55 - 0.01f)
		{
			for (int i = 0; i < 5; i++)
			{
				drawHidariGiri[i] = false;
			}
			finish = true;
			return;
		}

		position += velocity;
		naObject->SetPosition(position);

		//�R���C�_�[�X�V	
		Object::Update();
		naObject->Update();
	}
}