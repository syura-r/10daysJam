#pragma once
#include "DebugCamera.h"
#include "Object.h"
class Player :
	public Object
{
public:
	Player();
	~Player();
	void Initialize()override;
	void Update()override;
	void OnCollision(const CollisionInfo& info)override;
	void Draw() override;
	static void SetDebugCamera(DebugCamera* cameraPtr) { camera = cameraPtr; }
	static void SetLightCamera(LightCamera* cameraPtr) { lightCamera = cameraPtr; }

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
	float speed = 0.3f;
	Vector3 prePos;
	//�i�̃I�u�W�F�N�g
	Object* naObject = nullptr;
	//�i�s����(�E�������ǂ���)
	bool moveRight;
	//�i�s�������ς�������Ƀy���y��������t���O
	bool moveShift;
	//�U����Ԃ̎��
	enum ATTACKSTATE
	{
		NONE,//�U�����ĂȂ�
		MeleeAttack,//�ߐڍU��
		JumpAttack,//�󒆍U��
		Boomerang,//�u�[������
		ULT//���a��
	};
	//���݂̍U�����
	ATTACKSTATE nowAttackState;

	enum ANIMATIONSTATE
	{
		Wait,//�ҋ@�A�j���[�V����
		Walk,//����
		Jump,//�W�����v
		Turn,//�U�����
	};
	//���݂̃A�j���[�V�������
	ANIMATIONSTATE nowAnimationState;
	
	Object* hitObj = nullptr;
private://�ÓI�����o�ϐ�
	static DebugCamera* camera;
	static LightCamera* lightCamera;

};

