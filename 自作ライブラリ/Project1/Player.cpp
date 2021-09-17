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
	else if (Input::TriggerKey(DIK_SPACE))
	{
		onGround = false;
		const float jumpVYFist = 0.5f;//�W�����v�����������
		fallV = { 0,jumpVYFist,0,0 };

		//�����]���A�j���[�V�������ȊO�̓A�j���[�V�������Đ�
		if (nowAnimationState != Turn)
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

	//�R���C�_�[�X�V	
	Object::Update();
	//���R���C�_�[���擾
	BoxCollider* boxCollider = dynamic_cast<BoxCollider*>(collider);
	assert(boxCollider);

	//BoxCollider* boxCollider = dynamic_cast<BoxCollider*>(collider);
	//assert(boxCollider);
	//�N�G���[�R�[���o�b�N�N���X
	class PlayerQueryCallBack :public QueryCallback
	{
	public:
		PlayerQueryCallBack(Box* box) :box(box) {};

		//�Փˎ��̃R�[���o�b�N�֐�
		bool OnQueryHit(const QueryHit& info)
		{
			//���[���h�̏����
			const XMVECTOR up = { 0,1,0,0 };
			//�r�˕���
			XMVECTOR rejectDir = XMVector3Normalize(info.reject);
			//������Ɣr�˕����̊p�x���̃R�T�C���l
			float cos = XMVector3Dot(rejectDir, up).m128_f32[0];

			//�n�ʔ��肵�����l�p�x
			const float threshold = cosf(XMConvertToRadians(30.0f));
			//�p�x���ɂ���ēV��܂��͒n�ʂƔ��肳���ꍇ��������
			if (-threshold < cos && cos < threshold)
			{
				//����r��(�����o��)
				box->center += info.reject;
				move += info.reject;
			}
			return true;
		}
		//�N�G���[�Ɏg�p���鋅
		Box* box = nullptr;
		//�r�˂ɂ��ړ���(�ݐϒl)
		DirectX::XMVECTOR move = {};
	};

	//�N�G���[�R�[���o�b�N�̊֐��I�u�W�F�N�g
	PlayerQueryCallBack callback(boxCollider);
	//callback.OnQueryHit(info);
	//���ƒn�`�̌�����S����
	CollisionManager::GetInstance()->QueryBox(*boxCollider, &callback, COLLISION_ATTR_LANDSHAPE);

	//�����ɂ��r�˕��𓮂���
	position.x += callback.move.m128_f32[0];
	position.y += callback.move.m128_f32[1];
	position.z += callback.move.m128_f32[2];

	
	//�R���C�_�[�X�V	
	Object::Update();

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
		if (CollisionManager::GetInstance()->Raycast(collider,ray, COLLISION_ATTR_LANDSHAPE,
			&raycastHit, boxCollider->GetScale().y * 2.0f + absDistance))
		{
			onGround = true;
			position.y -= (raycastHit.distance - boxCollider->GetScale().y * 2.0f);
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
		if (CollisionManager::GetInstance()->Raycast(collider,ray, COLLISION_ATTR_LANDSHAPE,
			&raycastHit, boxCollider->GetScale().y * 2.0f))
		{
			//���n
			onGround = true;
			position.y -= (raycastHit.distance - boxCollider->GetScale().y * 2.0f);
			//�s��X�V�Ȃ�
			Object::Update();
		}

	if (prePos != position)
	{
		lightCamera->SetTarget(position + Vector3{0,1,0});
		camera->SetTarget(position + Vector3{0, 1, 0});
		naObject->SetPosition(position);

	}
	//�R���C�_�[�X�V	
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
//----------------------�ړ�����-------------------------------
	if (Input::DownKey(DIK_D) || Input::DownKey(DIK_A))
	{
		if (Input::DownKey(DIK_D))
		{
			//��]�����֘A�̏�����
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
			//��]�����֘A�̏�����
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
		
		//��]���A�W�����v���ȊO�͕��s�A�j���[�V���������s
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
	
//------------�����]�����̉�]����------------
	
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

//------------�W�����v�A�j���[�V�����̍Đ�����-------------------

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
