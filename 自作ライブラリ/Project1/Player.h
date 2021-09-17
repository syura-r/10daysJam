#pragma once
#include "CollisionPrimitive.h"
#include "DebugCamera.h"
#include "Object.h"
#include "QueryCallback.h"

class Player :
	public Object
{
public:
	//�U����Ԃ̎��
	enum ATTACKSTATE
	{
		NoAttack,//�U�����ĂȂ�
		MeleeAttack,//�ߐڍU��
		JumpAttack,//�󒆍U��
		Boomerang,//�u�[������
		ULT//���a��
	};

public:
	Player();
	~Player();
	void Initialize()override;
	void Update()override;
	void OnCollision(const CollisionInfo& info)override;
	void Draw() override;
	static void SetDebugCamera(DebugCamera* cameraPtr) { camera = cameraPtr; }
	static void SetLightCamera(LightCamera* cameraPtr) { lightCamera = cameraPtr; }
	ATTACKSTATE GetAttackState() { return nowAttackState; }
private:
	//�U������
	void Attack();
	//�ړ�����
	void Move();
	//�ڒn�t���O
	bool onGround = true;
	//�����x�N�g��
	XMVECTOR fallV;
	//�ړ����x
	float speed = 0.2f;
	Vector3 prePos;
	//�i�̃I�u�W�F�N�g
	Object* naObject = nullptr;
	Vector3 naPos;
	//�i�s����(�E�������ǂ���)
	bool moveRight;
	//�i�s�������ς�������Ƀy���y��������t���O
	bool moveShift;

	//���݂̍U�����
	ATTACKSTATE nowAttackState;

	enum ANIMATIONSTATE
	{
		Wait,//�ҋ@�A�j���[�V����
		Walk,//����
		Jump,//�W�����v
		Turn,//�U�����
		Attacks,//�U��
	};
	//���݂̃A�j���[�V�������
	ANIMATIONSTATE nowAnimationState;

	Object* hitObj = nullptr;

	//�U���t���O
	bool attackFrag;

	//�U������p�̃J�E���^
	int attackCounter;
	
//---------------�u�[������------------------
	//�u�[�������̔��˕���
	bool boomerangMoveRight;
	//�u�[�������̍ő�ړ����x
	const float BoomerangMaxSpeed = 0.3f;
	float  boomerangSpeed;
	//�u�[�������̌����܂ł̎���
	const int BoomerangTime = 15;
	//�u�[�����������܂ł̃J�E���^�[
	int boomerangCounter;
//-------------------------------------------

//------------�U���A�j���[�V�����p-----------
	Object* itiObject = nullptr;//���̈�p��(��̕���)
	Object* noObject = nullptr;//���̓�p��(�m�̕���)
	Object* eObject = nullptr;//���̎c��(�G�̕���)

	const Vector3 itiOffset = {};//���̃|�W�V��������ǂꂾ������Ă邩
	const Vector3 noOffset = {};
	const Vector3 eOffset = {};

	//�ߐڒi�K
	int meleeAttackStage;
	//2�i�ڈȍ~�p�������邩�̃t���O
	bool continueMeleeAttack;
//-------------------------------------------
private://�����N���X
	//�N�G���[�R�[���o�b�N�N���X
	class PlayerQueryCallBack :public QueryCallback
	{
	public:
		PlayerQueryCallBack(Box* box) :box(box) {};
		PlayerQueryCallBack(Sphere* sphere) :sphere(sphere) {};
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
				if(box)
				box->center += info.reject;
				if (sphere)
					sphere->center += info.reject;
				move += info.reject;
			}
			return true;
		}
		//�N�G���[�Ɏg�p���鋅
		Box* box = nullptr;
		Sphere* sphere = nullptr;

		//�r�˂ɂ��ړ���(�ݐϒl)
		DirectX::XMVECTOR move = {};
	};

private://�ÓI�����o�ϐ�
	static DebugCamera* camera;
	static LightCamera* lightCamera;

};

