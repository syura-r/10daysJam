#include"Player.h"
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
}

void Player::Initialize()
{
	name = typeid(*this).name();
	//scale = { 0.3f,1,0.5f };
	position = { 0,0,0 };
	rotation = { 90,0,0 };
	scale = { 0.5f, 0.5f, 0.5f };
	naObject->SetPosition(position);
	naObject->SetRotation(rotation);
	naObject->SetScale(scale);

	prePos = position;
	nowAttackState = NONE;
	lightCamera->SetTarget(position + Vector3{ 0,1,0 });
	camera->SetTarget(position + Vector3{ 0, 1, 0 });
	moveRight = true;
	moveShift = false;
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
	//else if (Input::TriggerKey(DIK_SPACE))
	//{
	//	onGround = false;
	//	const float jumpVYFist = 0.5f;//�W�����v�����������
	//	fallV = { 0,jumpVYFist,0,0 };
	//}
	
	//�R���C�_�[�X�V	
	Object::Update();
	naObject->Update();
	if (prePos != position)
	{
		lightCamera->SetTarget(position + Vector3{0,1,0});
		camera->SetTarget(position + Vector3{0, 1, 0});
	}
}

void Player::OnCollision(const CollisionInfo & info)
{
	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(collider);
	assert(sphereCollider);
	sphereCollider->SetRadius(0.50f);
	sphereCollider->SetOffset({ 0,0.5f,0 ,0 });
	//�N�G���[�R�[���o�b�N�N���X
	class PlayerQueryCallBack
	{
	public:
		PlayerQueryCallBack(Sphere* sphere) :sphere(sphere) {};

		//�Փˎ��̃R�[���o�b�N�֐�
		bool OnQueryHit(const CollisionInfo& info)
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
				sphere->center += info.reject;
				move += info.reject;
			}
			return true;
		}
		//�N�G���[�Ɏg�p���鋅
		Sphere* sphere = nullptr;
		//�r�˂ɂ��ړ���(�ݐϒl)
		DirectX::XMVECTOR move = {};
	};

	//�N�G���[�R�[���o�b�N�̊֐��I�u�W�F�N�g
	PlayerQueryCallBack callback(sphereCollider);
	callback.OnQueryHit(info);
	//���ƒn�`�̌�����S����
	//CollisionManager::GetInstance()->QuerySphere(*sphereCollider, &callback, COLLISION_ATTR_LANDSHAPE);

	//�����ɂ��r�˕��𓮂���
	position.x += callback.move.m128_f32[0];
	position.y += callback.move.m128_f32[1];
	position.z += callback.move.m128_f32[2];
}

void Player::Draw()
{
	DirectXLib::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Object::CustomDraw(true);
	naObject->CustomDraw(true);
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
			if (!moveRight)
			{
				moveRight = true;
				moveShift = true;
			}
			position.x += speed;
		}
		if (Input::DownKey(DIK_A))
		{
			if (moveRight)
			{
				moveRight = false;
				moveShift = true;
			}
			position.x -= speed;
		}

		if (nowAttackState == NONE)
			naObject->SetPosition(position);
	}
//--------------------------------------------------------------
	
//------------�����]�����̉�]����------------
	if(moveShift)
	{
		if(moveRight)
		{
			if (rotation.y > 0)
				rotation.y -= 15;
			else
			{
				moveShift = false;
				rotation.y = 0;
			}
		}
		else
		{
			if (rotation.y < 180)
				rotation.y += 15;
			else
			{
				moveShift = false;
				rotation.y = 180;
			}
		}

		naObject->SetRotation(rotation);
	}
//--------------------------------------------
}
