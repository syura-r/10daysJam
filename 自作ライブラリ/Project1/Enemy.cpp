#include "Enemy.h"

#include "BoxCollider.h"
#include "CollisionAttribute.h"
#include "FBXManager.h"
#include "Input.h"
#include"OBJLoader.h"
#include "Player.h"
#include "PtrDelete.h"
#include "RaycastHit.h"
#include"CollisionManager.h"
Player* Enemy::player = nullptr;
Enemy::Enemy(const Vector3& pos)
{
	Create(FBXManager::GetModel("Migi2"));
	position = pos;
	destruction = 0;
	scaleFactor = 1.0f;
	positionFactor = 2.0f;
	rotationFactor = 0.79f;
	tessellation = 4;
	color = { 0,0,0,1 };
	scale = { 0.3f,0.3f,0.3f };
	CreateConstBuff();
	BoxCollider* boxCollider = new BoxCollider({ 0,2.25f * scale.y,0,0 });
	boxCollider->SetScale(scale * 2);
	SetCollider(boxCollider);
	collider->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider->SetMove(true);

	hp = MaxHP;

	damageCounter = 0;


#ifdef _DEBUG
	hitBox = new Object();
	hitBox->Create(OBJLoader::GetModel("box"));
	hitBox->SetScale(scale * 4);
	hitBox->SetColor({ 1,1,1,0.4f });
	hitBox->SetPosition(position + Vector3{ 0, 2.25f * scale.y, 0 });
#endif

}

Enemy::~Enemy()
{
#ifdef _DEBUG
	PtrDelete(hitBox);
#endif
}

void Enemy::Update()
{
	if (dead)
		return;

	if (isDamage)
	{
		if (damageCounter >= invTime)
		{
			isDamage = false;
			damageCounter = 0;
			color = {0, 0, 0, 1};
		}
		damageCounter++;
	}
	if(playBreakAnimation)
	{
		destruction += breakSpeed;
		if (destruction >= 1.0f)
			dead = true;
	}
//----------------------------------------マップとの判定・排斥処理-------------------------------------------
	//コライダー更新
	Object::Update();
	//球コライダーを取得
	BoxCollider* boxCollider = dynamic_cast<BoxCollider*>(collider);
	assert(boxCollider);

	//球の上端から球の下端までのレイキャスト用レイを準備
	Ray ray;
	ray.start = boxCollider->center;
	ray.start.m128_f32[1] += boxCollider->GetScale().y;
	ray.dir = { 0,-1,0,0 };
	RaycastHit raycastHit;
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
	EnemyQueryCallBack callback(boxCollider);
	//球と地形の交差を全検索
	CollisionManager::GetInstance()->QueryBox(*boxCollider, &callback, COLLISION_ATTR_LANDSHAPE);


	//交差による排斥文を動かす
	position.x += callback.move.m128_f32[0];
	position.y += callback.move.m128_f32[1];
	position.z += callback.move.m128_f32[2];

//----------------------------------------------------------------------------------------------------------------

	Object::Update();
#ifdef _DEBUG
	hitBox->SetPosition(position + Vector3{ 0, 2.25f * scale.y, 0 });
	hitBox->Update();
#endif
}

void Enemy::Draw()
{
	// 定数バッファへデータ転送
	ConstBuffData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));
	constMap->_Destruction = destruction;
	constMap->_ScaleFactor = scaleFactor;
	constMap->_PositionFactor = positionFactor;
	constMap->_RotationFactor = rotationFactor;
	constMap->_Tessellation = tessellation;


	constBuff->Unmap(0, nullptr);

	//floor->Draw();
	DirectXLib::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	if (Object3D::GetDrawShadow())
	{
		// パイプラインステートの設定
		PipelineState::SetPipeline("PBShadowMap");
		DirectXLib::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, constBuff->GetGPUVirtualAddress());

	}
	else
	{
		// パイプラインステートの設定
		PipelineState::SetPipeline("PolygonBreak");
		DirectXLib::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(5, constBuff->GetGPUVirtualAddress());
	}
	CustomDraw(false, true, ALPHA, true);
#ifdef _DEBUG
	hitBox->Draw();
#endif

}

void Enemy::OnCollision(const CollisionInfo& info)
{
	BaseCollider* col = info.collider;
	if( isDamage || col->GetAttribute() != COLLISION_ATTR_ATTACK )
		return;

	int damage = 0;
	switch (player->GetAttackState())
	{
	case Player::Boomerang:
	{
		invTime = 15;
		damage = 50;
		break;
	}
	case Player::MeleeAttack:
	{
		invTime = 13;
		damage = 35;
		break;
	}
	case Player::JumpAttack:
	{
		invTime = 26;
		damage = 50;
		break;
	}
	case Player::ULT:
	{
		break;
	}
	default:
		return;
	}

	color = { 0.5f,0,0,1 };
	hp -= damage;
	if (hp <= 0)
		playBreakAnimation = true;
	isDamage = true;
}

void Enemy::CreateConstBuff()
{
	HRESULT result = DirectXLib::GetInstance()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBuffData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
}
