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
#include "ParticleEmitter.h"
#define SIZE  1.0f

Player* Enemy::player = nullptr;
Enemy::Enemy(const Vector3& pos, const Vector3& velocity)
{
	Create(FBXManager::GetModel("Migi2"));
	position = pos;
	destruction = 0;
	scaleFactor = 1.0f;
	positionFactor = 2.0f;
	rotationFactor = 0.79f;
	tessellation = 3;
	color = { 0,0,0,1 };
	scale = { 0.3f,0.3f,0.3f };
	CreateConstBuff();

	naObject = new Object();
	naObject->Create(FBXManager::GetModel("Migi1"));
	naObject->SetColor(color);
	naObject->SetScale(scale);
	
	BoxCollider* boxCollider = new BoxCollider({ 0,2.25f * scale.y,0,0 });
	boxCollider->SetScale(scale * 2);
	SetCollider(boxCollider);
	collider->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider->SetMove(true);
	this->velocity.x = velocity.x;
	fallV = {0, velocity.y,0,0 };
	hp = MaxHP;

	damageCounter = 0;

	if (pos.x > player->WallLeft)
		bossChild = true;
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
	PtrDelete(naObject);
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
		return;
	}
//----------------------------------------�}�b�v�Ƃ̔���E�r�ˏ���-------------------------------------------
	//�R���C�_�[�X�V
	Object::Update();
	//���R���C�_�[���擾
	BoxCollider* boxCollider = dynamic_cast<BoxCollider*>(collider);
	assert(boxCollider);

	//���̏�[���狅�̉��[�܂ł̃��C�L���X�g�p���C������
	Ray ray;
	ray.start = boxCollider->center;
	ray.start.m128_f32[1] += boxCollider->GetScale().y;
	ray.dir = { 0,-1,0,0 };
	RaycastHit raycastHit;
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

	//�ڒn���
	if (onGround)
	{
		firstGround = true;
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

	//if (EffekseerLib::GetEnd(handle))
	//{
	//	EffekseerLib::StopEffect(handle);
	//}


	//�N�G���[�R�[���o�b�N�̊֐��I�u�W�F�N�g
	EnemyQueryCallBack callback(boxCollider);
	//���ƒn�`�̌�����S����
	CollisionManager::GetInstance()->QueryBox(*boxCollider, &callback, COLLISION_ATTR_LANDSHAPE);
	
	//�����ɂ��r�˕��𓮂���
	position.x += callback.move.m128_f32[0];
	position.y += callback.move.m128_f32[1];
	position.z += callback.move.m128_f32[2];

	if (callback.move.m128_f32[0] < 0)
		speed = -0.02f;
	else if (callback.move.m128_f32[0] > 0)
		speed = 0.02f;

//----------------------------------------------------------------------------------------------------------------

	if (bossChild)
	{
		if (position.x < player->WallLeft)
		{
			position.x = player->WallLeft;
			speed *= -1.0f;
		}
		else if(position.x > player->WallRight)
		{
			position.x = player->WallRight;
			speed *= -1.0f;
		}
		if (firstGround)
			position.x += speed;
		else
			position += velocity;

	}
	else
	{
		if (position.x > player->WallLeft - 1)
		{
			position.x = player->WallLeft - 1;
			speed *= -1.0f;
		}
		if (position.x < 1)
		{
			position.x = 1;
			speed *= -1.0f;
		}

	}
	naObject->SetPosition(position);
	naObject->SetRotation(rotation);
	naObject->Update();
	Object::Update();
#ifdef _DEBUG
	hitBox->SetPosition(position + Vector3{ 0, 2.25f * scale.y, 0 });
	hitBox->Update();
#endif
}

void Enemy::Draw()
{
	// �萔�o�b�t�@�փf�[�^�]��
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
		// �p�C�v���C���X�e�[�g�̐ݒ�
		PipelineState::SetPipeline("PBShadowMap");
		DirectXLib::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, constBuff->GetGPUVirtualAddress());
	}
	else
	{
		// �p�C�v���C���X�e�[�g�̐ݒ�
		PipelineState::SetPipeline("PolygonBreak");
		DirectXLib::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(5, constBuff->GetGPUVirtualAddress());
	}
	CustomDraw(false, true, ALPHA, true);
	naObject->CustomDraw(false, true, ALPHA, true);
#ifdef _DEBUG
	//hitBox->Draw();
#endif
}

void Enemy::OnCollision(const CollisionInfo& info)
{
	BaseCollider* col = info.collider;
	if( isDamage || col->GetAttribute() != COLLISION_ATTR_ATTACK )
		return;

	int damage = 0;
	Vector3 effectPos = position + Vector3{0,0.8f,-0.6f};
	switch (player->GetAttackState())
	{
	case Player::Boomerang:
	{
		invTime = 25;
		damage = 35;
		ParticleEmitter::CreateSlashEffects(effectPos, std::rand() % 180, { 0.5f,0.5f,1 });
		break;
	}
	case Player::MeleeAttack:
	{
		invTime = 13;
		damage = 100;
		ParticleEmitter::CreateSlashEffects(effectPos, std::rand() % 180, { 0.5f,0.5f,1 });

		break;
	}
	case Player::JumpAttack:
	{
		invTime = 26;
		damage = 100;
		ParticleEmitter::CreateWindEffects(effectPos, { 0.5f,0.5f,1 });
		break;
	}
	case Player::ULT:
		break;
	default:
		return;
	}
	color = { 0.5f,0,0,1 };
	hp -= damage;
	if (hp <= 0)
	{
		playBreakAnimation = true;
		ParticleEmitter::CreateRiseEffects(effectPos, { 1,0,0 });
		collider->SetAttribute(COLLISION_ATTR_ALLIES);
		Object::Update();
	}
	isDamage = true;
}

void Enemy::CreateConstBuff()
{
	HRESULT result = DirectXLib::GetInstance()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBuffData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
}
