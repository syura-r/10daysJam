#pragma once

#include "CollisionPrimitive.h"
#include "DebugCamera.h"
#include "FbxLoader.h"
#include "Object.h"
#include "QueryCallback.h"
#include "Sprite.h"

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
	bool GetWaitFight() { return waitFight; }
	static void SetDebugCamera(DebugCamera* cameraPtr) { camera = cameraPtr; }
	static void SetLightCamera(LightCamera* cameraPtr) { lightCamera = cameraPtr; }
	const ATTACKSTATE& GetAttackState() { return nowAttackState; }
	const bool& GetStartFight() { return startFight; }
	inline void StartFight()
	{
		startFight = true;
		notMove = false;
		waitFight = false;
	}
private://�����o�֐�
	//�U������
	void Attack();
	//�ړ�����
	void Move();

private://�����o�ϐ�
	//�ڒn�t���O
	bool onGround = true;
	//�����x�N�g��
	XMVECTOR fallV;
	//�ړ����x
	float speed = 0.1f;
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

	//�A�j���[�V�����̎��
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

	//�U���t���O
	bool attackFrag;

	//�U������p�̃J�E���^
	int attackCounter;

	int hp;

	//������_���[�W�����󂯂�̂�h���t���O
	bool damage;
	//���G����
	int invCounter;


	//HP�p�̃X�v���C�g
	Sprite* hpSprites[4];

//-------------------�{�X���o�p--------------------
	bool notMove;//�퓬�J�n�O�̎����ړ��t���O
	bool startFight;//�{�X��J�n�t���O
	bool waitFight;//�A�j���[�V�����I����{�X�̏o���҂��t���O
public:
	const float WallLeft = 40;//�����̃J�x
	const float WallRight = 60;//�E���̃J�x
private:
	bool drawBlackTex;//�т��o���t���O
	Sprite* upTex;//�㕔�ɏo�Ă��鍕����
	Sprite* downTex;//�����ɏo�Ă��鍕����
	float texSizeY;//�㉺�̑т̏c�T�C�Y
	int texSizeCounter;//�C�[�W���O�p�̃J�E���^�[
//-------------------------------------------------	
//-------------------�f�o�b�O�p-------------------
#ifdef _DEBUG
	Object* hitObj = nullptr;
	Vector3 rejectVal = {}; 
#endif _DEBUG
//------------------------------------------------

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

	bool boomerang;
//-------------------------------------------

//------------�U���A�j���[�V�����p-----------
	Object* itiObject = nullptr;//���̈�p��(��̕���)
	Object* noObject = nullptr;//���̓�p��(�m�̕���)
	Object* eObject = nullptr;//���̎c��(�G�̕���)
	Object* zanzoObject = nullptr;//�c��

	//�c���G�t�F�N�g�̕\�t���O
	bool drawZanzoFrag;
	
	//�ߐڒi�K
	int meleeAttackStage;
	//2�i�ڈȍ~�p�������邩�̃t���O
	bool continueMeleeAttack;


	Object* hidariGiri[5] = {};

	bool drawHidariGiri[5];
	FbxTime drawFrame[5];

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
			//if (-threshold < cos && cos < threshold)
			//{
				///����r��(�����o��)
			if (box)
				box->center += info.reject;
			if (sphere)
				sphere->center += info.reject;
			move += info.reject;

			//}
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

