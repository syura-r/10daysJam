#include"Player.h"

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
DebugCamera* Player::camera = nullptr;
LightCamera* Player::lightCamera = nullptr;

Player::Player()
{
	Create(FBXManager::GetModel("Hidari2"));
	naObject = new Object();
	naObject->Create(FBXManager::GetModel("Hidari1"));
	Initialize();

	BoxCollider* boxCollider = new BoxCollider({ 0,2.25f * scale.y,0,0 }, scale * 2);
	SetCollider(boxCollider);
	collider->SetAttribute(COLLISION_ATTR_ALLIES);
	collider->SetMove(true);

	itiObject = new Object();
	itiObject->Create(FBXManager::GetModel("SwoedMode_1"));
	itiObject->SetScale(scale);
	itiObject->SetColor(color);
	
	noObject = new Object();
	noObject->Create(FBXManager::GetModel("SwoedMode_2"));
	noObject->SetScale(scale);
	noObject->SetColor(color);

	eObject = new Object();
	eObject->Create(FBXManager::GetModel("SwoedMode_3"));
	eObject->SetScale(scale);
	eObject->SetColor(color);


	
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
	PtrDelete(itiObject);
	PtrDelete(noObject);
	PtrDelete(eObject);
	PtrDelete(naObject);
	
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
	//naObject->SetColor(color);

	prePos = position;
	nowAttackState = NoAttack;
	lightCamera->SetTarget(position + Vector3{ 0,1,0 });
	camera->SetTarget(position + Vector3{ 0, 1, 0 });
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
}

void Player::Update()
{
	Move();
	Attack();
//-----------------------------ダメージ処理-----------------------------------------
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
	
	//落下処理
	if (!onGround)
	{
		//下向き加速
		const float fallAcc = -0.02f;
		const float fallVYMin = -0.5f;
		//加速
		fallV.m128_f32[1] = max(fallV.m128_f32[1] + fallAcc, fallVYMin);
		//移動
		position.x += fallV.m128_f32[0];
		position.y += fallV.m128_f32[1];
		position.z += fallV.m128_f32[2];
	}
	//ジャンプ動作(地面作るまでコメントアウト)
	else if (Input::TriggerKey(DIK_SPACE))
	{
		onGround = false;
		const float jumpVYFist = 0.5f;//ジャンプ時上向き初速
		fallV = { 0,jumpVYFist,0,0 };

		//方向転換アニメーション時以外はアニメーションを再生
		if (nowAnimationState != Turn)
		{
			//シャンプアニメーションん初期化処理
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
//----------------------------------------マップとの判定・排斥処理-------------------------------------------
	//コライダー更新
	Object::Update();
	//コライダーを取得
	BoxCollider* boxCollider = dynamic_cast<BoxCollider*>(collider);
	assert(boxCollider);

	//球の上端から球の下端までのレイキャスト用レイを準備
	Ray ray;
	ray.start = boxCollider->center;
	ray.start.m128_f32[1] += boxCollider->GetScale().y;
	ray.dir = { 0,-1,0,0 };
	RaycastHit raycastHit;

	//接地状態
	if (onGround)
	{
		//スムーズに坂を下る為の吸着距離
		const float absDistance = 0.2f;
		//接地を維持
		if (CollisionManager::GetInstance()->Raycast(collider, ray, COLLISION_ATTR_LANDSHAPE,
			&raycastHit, boxCollider->GetScale().y * 2.0f + absDistance))
		{
			onGround = true;
			position.y -= raycastHit.distance - boxCollider->GetScale().y * 2.0f;
			//行列更新など
			Object::Update();
		}
		//地面がないので落下
		else {
			onGround = false;
			fallV = {};
		}
	}
	//落下状態
	else if (fallV.m128_f32[1] <= 0.0f)
		if (CollisionManager::GetInstance()->Raycast(collider, ray, COLLISION_ATTR_LANDSHAPE,
			&raycastHit, boxCollider->GetScale().y * 2.0f))
		{
			//着地
			onGround = true;
			position.y -= raycastHit.distance - boxCollider->GetScale().y * 2.0f;
			//行列更新など
			Object::Update();
		}

	

	//クエリーコールバックの関数オブジェクト
	PlayerQueryCallBack callback(boxCollider);
	//球と地形の交差を全検索
	CollisionManager::GetInstance()->QueryBox(*boxCollider, &callback, COLLISION_ATTR_LANDSHAPE);

	if (callback.move.m128_f32[0] < -speed)
		callback.move.m128_f32[0] = -speed;
	else if (callback.move.m128_f32[0] > speed)
		callback.move.m128_f32[0] = speed;
	//交差による排斥文を動かす
	position.x += callback.move.m128_f32[0];
	position.y += callback.move.m128_f32[1];
	position.z += callback.move.m128_f32[2];
	if (callback.move.m128_f32[1] < -0.1f)
		fallV.m128_f32[1] = 0;
	rejectVal.x = callback.move.m128_f32[0];
	rejectVal.y = callback.move.m128_f32[1];
	rejectVal.z = callback.move.m128_f32[2];

	if(rejectVal.y != 0)
	{
		printf("rejectVal.y : %f\n", rejectVal.y);
	}
	if (rejectVal.x != 0)
	{
		printf("rejectVal.x : %f\n", rejectVal.x);
	}

	//排斥で地面に立ってる場合
	if (!onGround && prePos.y == position.y && callback.move.m128_f32[1] != 0)
		onGround = true;
	//コライダー更新	
	Object::Update();

//----------------------------------------------------------------------------------------------------------------
	
	if (prePos != position)//カメラのターゲット更新
	{
		lightCamera->SetTarget(position + Vector3{ 0,1,0 });
		camera->SetTarget(position + Vector3{ 0, 0, 0 });
	}

	if (nowAttackState != Boomerang)
	{
		naPos = position;
	}
	
	if(nowAttackState == MeleeAttack || nowAttackState == JumpAttack)
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

	//コライダー更新	
	Object::Update();
	naObject->Update();



#ifdef _DEBUG
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
	if(damage || nowAttackState == JumpAttack || info.collider->GetAttribute() != COLLISION_ATTR_ENEMYS)
		return;

	color = { 0.4f,0,0,1 };
	hp--;
	if (hp <= 0)
		dead = true;
	damage = true;
	invCounter = 0;
}

void Player::Draw()
{
	ImGui::Begin("PlayerState");
	ImGui::Text("onGround : %d\n", onGround);
	ImGui::Text("position.y : %f\n", position.y);
	ImGui::Text("fallVelY : %f\n", fallV.m128_f32[1]); 
	ImGui::Text("rejectVal.y : %f\n", rejectVal.y);

	ImGui::End();
	DirectXLib::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if(nowAttackState == MeleeAttack || nowAttackState == JumpAttack)
	{
		itiObject->CustomDraw(true);
		noObject->CustomDraw(true);
		eObject->CustomDraw(true);
		return;
	}
	CustomDraw(true);
	naObject->CustomDraw(true);
	
#ifdef _DEBUG
	//if (nowAttackState == JumpAttack)
		//hitObj->Draw();
#endif _DEBUG

}

void Player::Attack()
{
//-----------------------------------------攻撃発生処理---------------------------------------------
	if (!attackFrag)
	{
		if (nowAttackState == NoAttack && Input::TriggerKey(DIK_RETURN))
		{
			//----------------------------通常攻撃の発生処理----------------------------------------
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
				if (!moveRight)
					rotation.y = 180;
				else
					rotation.y = 0;
			}//-------------------------------------------------------------------------------------
			//----------------------------ジャンプ攻撃の発生処理------------------------------------
			else
			{
				nowAttackState = JumpAttack;
				nowAnimationState = Attacks;
				attackFrag = true;
				FBXManager::GetModel("SwoedMode_1")->SetAnimationFrame(101, 130);
				FBXManager::GetModel("SwoedMode_2")->SetAnimationFrame(101, 130);
				FBXManager::GetModel("SwoedMode_3")->SetAnimationFrame(101, 130);


			}//-------------------------------------------------------------------------------------

		}
		
		//----------------------------ブーメランの発生処理------------------------------------------
		if (Input::TriggerKey(DIK_Z))
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

			BoxCollider* boxCollider = dynamic_cast<BoxCollider*>(collider);
			assert(boxCollider);

			boxCollider->SetScale({ scale.x * 2,scale.y ,scale.z * 2 });
			boxCollider->SetOffset({ 0,2.25f * scale.y / 2 + 0.025f ,0,0 });
			boxCollider->Update();


		}//-------------------------------------------------------------------------------------
	}
//----------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------攻撃更新処理------------------------------------------------------------
	else
	{
		switch (nowAttackState)
		{
		case MeleeAttack://---------------------------------通常攻撃----------------------------------------------------------
		{
			auto nowAnimationTime = FBXManager::GetModel("SwoedMode_1")->GetCurrentAnimationTime();
			FbxTime beginTime;
			FbxTime endTime;
			switch (meleeAttackStage)
			{
			case 1://-----------------------------------------1段階目----------------------------------------------------------
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
					//クエリーコールバックの関数オブジェクト
					PlayerQueryCallBack callback(boxCollider);
					CollisionManager::GetInstance()->QueryBox(*boxCollider, &callback, COLLISION_ATTR_ENEMYS, boxCollider);
				}
				if( Input::TriggerKey(DIK_RETURN))
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
						meleeAttackStage = 2;
					}
					else
					{
						attackFrag = false;
						nowAttackState = NoAttack;
						nowAnimationState = Wait;
					}
				}
				break;
			}//---------------------------------------------------------------------------------------------------------------
			case 2://----------------------------------------2段階目----------------------------------------------------------
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
					//クエリーコールバックの関数オブジェクト
					PlayerQueryCallBack callback(boxCollider);
					CollisionManager::GetInstance()->QueryBox(*boxCollider, &callback, COLLISION_ATTR_ENEMYS, boxCollider);
				}
				else if ( Input::TriggerKey(DIK_RETURN))
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
						meleeAttackStage = 3;
					}
					else
					{
						attackFrag = false;
						nowAttackState = NoAttack;
						nowAnimationState = Wait;
					}
				}

				break;
			}//---------------------------------------------------------------------------------------------------------------
			case 3://----------------------------------------3段階目----------------------------------------------------------
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
					//クエリーコールバックの関数オブジェクト
					PlayerQueryCallBack callback(boxCollider);
					CollisionManager::GetInstance()->QueryBox(*boxCollider, &callback, COLLISION_ATTR_ENEMYS, boxCollider);
				}
				FBXManager::GetModel("SwoedMode_2")->PlayAnimation();
				FBXManager::GetModel("SwoedMode_3")->PlayAnimation();
				if (!FBXManager::GetModel("SwoedMode_1")->PlayAnimation())
				{
					attackFrag = false;
					nowAttackState = NoAttack;
					nowAnimationState = Wait;

				}

				break;
			}//---------------------------------------------------------------------------------------------------------------
			default:
				break;
			}	
			break;
		}//----------------------------------------------------------------------------------------------------------------------
		case Boomerang://---------------------------------------ブーメラン---------------------------------------------------------
		{
			SphereCollider* sphereCollider = new SphereCollider({ 0,2.25f * scale.y,0,0 }, 0.5f);
			sphereCollider->SetAttribute(COLLISION_ATTR_ATTACK);
			sphereCollider->SetObject(naObject);
			sphereCollider->Update();
			//クエリーコールバックの関数オブジェクト
			PlayerQueryCallBack callback(sphereCollider);
			CollisionManager::GetInstance()->QuerySphere(*sphereCollider, &callback, COLLISION_ATTR_ENEMYS, sphereCollider);

			if (boomerangCounter >= BoomerangTime * 4)
			{
				attackFrag = false;
				nowAttackState = NoAttack;
				nowAnimationState = Wait;
				BoxCollider* boxCollider = dynamic_cast<BoxCollider*>(collider);
				assert(boxCollider);

				boxCollider->SetScale(scale * 2);
				boxCollider->SetOffset({ 0,2.25f * scale.y,0,0 });
				boxCollider->Update();
			}
			break;
		}//----------------------------------------------------------------------------------------------------------------------
		case JumpAttack://---------------------------------------ジャンプ攻撃----------------------------------------------------
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
				//クエリーコールバックの関数オブジェクト
				PlayerQueryCallBack callback(sphereCollider);
				CollisionManager::GetInstance()->QuerySphere(*sphereCollider, &callback, COLLISION_ATTR_ENEMYS, sphereCollider);
			}
			FBXManager::GetModel("SwoedMode_1")->PlayAnimation();
			FBXManager::GetModel("SwoedMode_2")->PlayAnimation();

			if(!FBXManager::GetModel("SwoedMode_3")->PlayAnimation())
			{
				attackFrag = false;
				nowAttackState = NoAttack;
				nowAnimationState = Wait;
			}
			break;
		}//----------------------------------------------------------------------------------------------------------------------
		case ULT://-------------------------------------------------左斬り-------------------------------------------------------
		{
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
//-----------------------------------------移動処理---------------------------------------------
	if (nowAttackState != MeleeAttack && (Input::DownKey(DIK_D) || Input::DownKey(DIK_A)))
	{
		if (Input::DownKey(DIK_D))
		{
			//回転処理関連の初期化
			if (!moveRight)
			{
				rotation.y = 180;
				moveRight = true;
				nowAnimationState = Turn;
				FBXManager::GetModel("Hidari2")->SetAnimationFrame(61, 120, 3);
				FBXManager::GetModel("Hidari1")->SetAnimationFrame(61, 120, 3);
				naObject->SetRotation(rotation);

			}
			position.x += speed;
			if (nowAttackState == Boomerang)
				naPos.x += speed;
		}
		if (Input::DownKey(DIK_A))
		{
			//回転処理関連の初期化
			if (moveRight)
			{
				rotation.y = 0;
				moveRight = false;
				nowAnimationState = Turn;
				FBXManager::GetModel("Hidari2")->SetAnimationFrame(61, 120, 3);
				FBXManager::GetModel("Hidari1")->SetAnimationFrame(61, 120, 3);
				naObject->SetRotation(rotation);
			}
			position.x -= speed;
			if (nowAttackState == Boomerang)
				naPos.x -= speed;
		}

		//待機アニメーションの時は歩行アニメーションを実行
		if (nowAnimationState == Wait)
		{
			FBXManager::GetModel("Hidari2")->SetAnimationFrame(0, 60, 2);
			FBXManager::GetModel("Hidari1")->SetAnimationFrame(0, 60, 2);
			nowAnimationState = Walk;
		}
		if (nowAnimationState == Walk)
		{
			FBXManager::GetModel("Hidari2")->PlayAnimation(true);
			FBXManager::GetModel("Hidari1")->PlayAnimation(true);
		}
	}
//---------------------------------------------------------------------------------------------------

//------------------------------------方向転換時の回転処理---------------------------------------

	if (nowAnimationState == Turn)
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
//-----------------------------------------------------------------------------------------------

//-----------------------------ジャンプアニメーションの再生処理----------------------------------

	if (nowAnimationState == Jump)
	{
		FBXManager::GetModel("Hidari2")->PlayAnimation();
		if (!FBXManager::GetModel("Hidari1")->PlayAnimation())
		{
			nowAnimationState = Wait;
		}
	}

//------------------------------------------------------------------------------------------------


//---------------------------------ブーメランの移動処理--------------------------------------------
	if (nowAttackState == Boomerang)
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
}
