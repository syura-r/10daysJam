#include "Enemy.h"

#include "BoxCollider.h"
#include "CollisionAttribute.h"
#include "FBXManager.h"
#include "Input.h"
#include"OBJLoader.h"
#include "Player.h"
#include "PtrDelete.h"

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
	BoxCollider* boxCollider = new BoxCollider({ 0,0.45f,0,0 });
	boxCollider->SetScale(scale * 2);
	SetCollider(boxCollider);
	collider->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider->SetMove(true);

	hp = MaxHP;

	hitBox = new Object();
	hitBox->Create(OBJLoader::GetModel("box"));
	hitBox->SetScale(scale * 4);
	hitBox->SetColor({ 1,1,1,0.4f });
	hitBox->SetPosition(position + Vector3{0, 0.45f, 0});

	damageCounter = 0;
}

Enemy::~Enemy()
{
	PtrDelete(hitBox);
}

void Enemy::Update()
{
	if (dead)
		return;

	if (isDamage)
	{
		if (damageCounter >= 10)
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
	Object::Update();
	hitBox->Update();
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

	hitBox->Draw();
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
		damage = 50;
		break;
	}
	case Player::MeleeAttack:
	{
		damage = 0;
		break;
	}
	case Player::JumpAttack:
	{
		break;
	}
	case Player::ULT:
	{
		break;
	}
	default:
		return;
	}

	color = { 1,0,0,1 };
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
