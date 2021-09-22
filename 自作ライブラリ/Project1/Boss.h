#pragma once
#pragma once
#include "Boss.h"
#include "CollisionPrimitive.h"
#include "Object.h"
#include "QueryCallback.h"
#include "Sprite.h"

class Player;
class Boss :
	public Object
{
public:
	Boss(const Vector3& pos);
	~Boss();
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnCollision(const CollisionInfo& info) override;

	static void SetPlayer(Player* playerPtr) { Boss::player = playerPtr; }
private:
	void Move();
	void Attack();
	void CreateConstBuff();
	struct ConstBuffData
	{
		float _Destruction; //����x����
		float _ScaleFactor; //�X�P�[���̕ω���
		float _PositionFactor;//�|�W�V�����̕ω���
		float _RotationFactor;//��]�̕ω���
		int _Tessellation;//�|���S�������x
		int _OnEasing;//�C�[�W���O�ŕ������邩
	};

	//�{�X�̏��
	enum State
	{
		WAIT,//�������ĂȂ�
		MOVE,//�U���O��̈ړ�
		ATTACK,//�U��
	};
	
	// ���݂̏��
	State nowState;
	
	//�U���̎��
	enum AttackType
	{
		VerticallyRush,
		WaveRush,
		SideRush,
		BirthChildren,
		
	};
	//���̍U���̎��
	AttackType attackType;
	//�����ʒu
	Vector3 BasePos;
	//�h��Ă���Ƃ��̂ǂ��ŉ����x��ς��邩�����ʒu
	Vector2 AfterPos;
	//�U���^�C�~���O���v��J�E���^�[
	int attackCounter;
	//�U���̃^�C�~���O�������t���O
	bool attack;
	//�U���O��ňړ����鎞�̖ړI�n
	Vector3 destination;
	//�U���O��ňړ����鎞�̑��x
	Vector3 toDestinationVel;
	
	struct Alert
	{
		//�x���p�̃X�v���C�g
		Sprite* alertTex = nullptr;
		//�x���t���O
		bool drawAlert;
		//�x���X�v���C�g�̕`��ʒu
		Vector2 pos;
		float alpha;
		//�_�ŗp�̃J�E���^�[
		int counter;
	};
	Alert alerts[2];
	
	//�����ʒu
	Vector3 InitPos;
	
	//���S�A�j���[�V�����t���O
	bool playBreakAnimation = false;
	//���S�A�j���[�V�����̑��x
	const float breakSpeed = 0.01f;
	
	//�_���[�W�t���O
	bool isDamage = false;
	int hp;
	const int MaxHP = 2000;
	//������q�b�g��h���J�E���^�[
	int damageCounter;
	//�U�����󂯂Ă���̖��G����(�󂯂��Z���Ƃɕς��邽�ߕϐ�)
	int invTime = 0;

	//�o���A�j���[�V�����̑��x
	const float createSpeed = 0.004f;

	//�o���A�j���[�V�����̊J�n�t���O
	bool appearAnimation;
	//�o�����
	bool appear;
	//�o���㔒�����点�邽�߂̃J�E���^�[
	int whiteCounter;
	
	//���x
	float speed = 0.02f;

	//1�p�ڂ�2�p�ڂ̃I�u�W�F�N�g
	Object* naObject = nullptr;
	Vector3 naPos;
	
#ifdef _DEBUG
	Object* hitBox = nullptr;
#endif
	ComPtr<ID3D12Resource> constBuff; // �萔�o�b�t�@
	float destruction;
	float scaleFactor; //�X�P�[���̕ω���
	float positionFactor;//�|�W�V�����̕ω���
	float rotationFactor;//��]�̕ω���
	int  tessellation;//�|���S�������x
	bool  onEasing;//�|���S�������x

	//�N�G���[�R�[���o�b�N�N���X
	class EnemyQueryCallBack :public QueryCallback
	{
	public:
		EnemyQueryCallBack(Box* box) :box(box) {};
		//�Փˎ��̃R�[���o�b�N�֐�
		bool OnQueryHit(const QueryHit& info)
		{
			//�{�b�N�X��r��(�����o��)
			if (box)
				box->center += info.reject;
			move += info.reject;

			return true;
		}
		//�N�G���[�Ɏg�p���鋅
		Box* box = nullptr;
		//�r�˂ɂ��ړ���(�ݐϒl)
		XMVECTOR move = {};
	};

	bool onGround = false;
	XMVECTOR fallV = {};


private://�ÓI�����o�ϐ�
	//�v���C���[�̃|�C���^
	static Player* player;
};



