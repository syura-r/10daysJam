#include"Player.h"

#include "BoxCollider.h"
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
}

Player::~Player()
{
	PtrDelete(naObject);
	PtrDelete(hitObj);

}

void Player::Initialize()
{
	name = typeid(*this).name();
	//scale = { 0.3f,1,0.5f };
	position = { 3,10,0 };
	//rotation = { 90,0,0 };
	scale = { 0.2f, 0.2f, 0.2f };
	color = { 0,0,0,1 };
	naObject->SetPosition(position);
	//naObject->SetRotation(rotation);
	naObject->SetScale(scale);
	naObject->SetColor(color);

	prePos = position;
	nowAttackState = NONE;
	lightCamera->SetTarget(position + Vector3{ 0,1,0 });
	camera->SetTarget(position + Vector3{ 0, 1, 0 });
	moveRight = true;
	moveShift = false;

	BoxCollider* boxCollider = new BoxCollider({0,0.45f,0,0});
	boxCollider->SetScale( scale * 2);
	SetCollider(boxCollider);
	collider->SetAttribute(COLLISION_ATTR_ALLIES);
	collider->SetMove(true);

	hitObj = new Object();
	hitObj->Create(OBJLoader::GetModel("box"));
	hitObj->SetScale(scale * 4);
	hitObj->SetColor({ 1,1,1,0.4f });

	nowAnimationState = Wait;
}

void Player::Update()
{
	Move();
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

	//コライダー更新	
	Object::Update();
	//球コライダーを取得
	BoxCollider* boxCollider = dynamic_cast<BoxCollider*>(collider);
	assert(boxCollider);

	//BoxCollider* boxCollider = dynamic_cast<BoxCollider*>(collider);
	//assert(boxCollider);
	//クエリーコールバッククラス
	class PlayerQueryCallBack :public QueryCallback
	{
	public:
		PlayerQueryCallBack(Box* box) :box(box) {};

		//衝突時のコールバック関数
		bool OnQueryHit(const QueryHit& info)
		{
			//ワールドの上方向
			const XMVECTOR up = { 0,1,0,0 };
			//排斥方向
			XMVECTOR rejectDir = XMVector3Normalize(info.reject);
			//上方向と排斥方向の角度差のコサイン値
			float cos = XMVector3Dot(rejectDir, up).m128_f32[0];

			//地面判定しきい値角度
			const float threshold = cosf(XMConvertToRadians(30.0f));
			//角度差によって天井または地面と判定される場合を除いて
			if (-threshold < cos && cos < threshold)
			{
				//球を排斥(押し出す)
				box->center += info.reject;
				move += info.reject;
			}
			return true;
		}
		//クエリーに使用する球
		Box* box = nullptr;
		//排斥による移動量(累積値)
		DirectX::XMVECTOR move = {};
	};

	//クエリーコールバックの関数オブジェクト
	PlayerQueryCallBack callback(boxCollider);
	//callback.OnQueryHit(info);
	//球と地形の交差を全検索
	CollisionManager::GetInstance()->QueryBox(*boxCollider, &callback, COLLISION_ATTR_LANDSHAPE);

	//交差による排斥文を動かす
	position.x += callback.move.m128_f32[0];
	position.y += callback.move.m128_f32[1];
	position.z += callback.move.m128_f32[2];

	
	//コライダー更新	
	Object::Update();

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
		if (CollisionManager::GetInstance()->Raycast(collider,ray, COLLISION_ATTR_LANDSHAPE,
			&raycastHit, boxCollider->GetScale().y * 2.0f + absDistance))
		{
			onGround = true;
			position.y -= (raycastHit.distance - boxCollider->GetScale().y * 2.0f);
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
		if (CollisionManager::GetInstance()->Raycast(collider,ray, COLLISION_ATTR_LANDSHAPE,
			&raycastHit, boxCollider->GetScale().y * 2.0f))
		{
			//着地
			onGround = true;
			position.y -= (raycastHit.distance - boxCollider->GetScale().y * 2.0f);
			//行列更新など
			Object::Update();
		}

	if (prePos != position)
	{
		lightCamera->SetTarget(position + Vector3{0,1,0});
		camera->SetTarget(position + Vector3{0, 1, 0});
		naObject->SetPosition(position);

	}
	//コライダー更新	
	Object::Update();
	naObject->Update();

	hitObj->SetPosition(position + Vector3{0,0.45f,0});
	hitObj->Update();
}

void Player::OnCollision(const CollisionInfo & info)
{
}

void Player::Draw()
{
	DirectXLib::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Object::CustomDraw(true);
	naObject->CustomDraw(true);
	//hitObj->Draw();
}

void Player::Attack()
{
	
}

void Player::Move()
{
	prePos = position;
//----------------------移動処理-------------------------------
	if (Input::DownKey(DIK_D) || Input::DownKey(DIK_A))
	{
		if (Input::DownKey(DIK_D))
		{
			//回転処理関連の初期化
			if (!moveRight)
			{
				rotation.y = 0;
				moveRight = true;
				nowAnimationState = Turn;
				FBXManager::GetModel("Hidari2")->SetAnimationFrame(120, 61,2);
				FBXManager::GetModel("Hidari1")->SetAnimationFrame(120, 61,2);
				naObject->SetRotation(rotation);

			}
			position.x += speed;
		}
		if (Input::DownKey(DIK_A))
		{
			//回転処理関連の初期化
			if (moveRight)
			{
				rotation.y = 0;
				moveRight = false;
				nowAnimationState = Turn;
				FBXManager::GetModel("Hidari2")->SetAnimationFrame(61, 120,2);
				FBXManager::GetModel("Hidari1")->SetAnimationFrame(61, 120,2);
				naObject->SetRotation(rotation);


			}
			position.x -= speed;
		}
		
		//回転時、ジャンプ時以外は歩行アニメーションを実行
		if (nowAnimationState != Walk && nowAnimationState != Turn && nowAnimationState != Jump)
		{
			FBXManager::GetModel("Hidari2")->SetAnimationFrame(0, 60, 2);
			FBXManager::GetModel("Hidari1")->SetAnimationFrame(0, 60, 2);
			nowAnimationState = Walk;
			if (moveRight)
			{
				rotation.y = 0;
				naObject->SetRotation(rotation);
			}
		}
		if (nowAnimationState == Walk)
		{
			FBXManager::GetModel("Hidari2")->PlayAnimation(true);
			FBXManager::GetModel("Hidari1")->PlayAnimation(true);
		}

	}
//--------------------------------------------------------------
	
//------------方向転換時の回転処理------------
	
	if(nowAnimationState == Turn)
	{
		FBXManager::GetModel("Hidari2")->PlayAnimation();
		if (!FBXManager::GetModel("Hidari1")->PlayAnimation())
		{
			nowAnimationState = Wait;
			rotation.y = 180;
			naObject->SetRotation(rotation);

		}
	}
//--------------------------------------------

//------------ジャンプアニメーションの再生処理-------------------

	if(nowAnimationState == Jump)
	{
		FBXManager::GetModel("Hidari2")->PlayAnimation();
		if (!FBXManager::GetModel("Hidari1")->PlayAnimation())
		{
			nowAnimationState = Wait;
		}

		
	}

	
//---------------------------------------------------------------
}
