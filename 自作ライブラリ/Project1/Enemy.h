#pragma once
#include "CollisionPrimitive.h"
#include "Object.h"
#include "QueryCallback.h"

class Player;
class Enemy :
	public Object
{
public:
	Enemy(const Vector3& pos);
	~Enemy();
	void Update() override;
	void Draw() override;
	void OnCollision(const CollisionInfo& info) override;

	static void SetPlayer(Player* playerPtr) { Enemy::player = playerPtr; }
private:
	void CreateConstBuff();
	struct ConstBuffData
	{
		float _Destruction; //����x����
		float _ScaleFactor; //�X�P�[���̕ω���
		float _PositionFactor;//�|�W�V�����̕ω���
		float _RotationFactor;//��]�̕ω���
		int _Tessellation;//�|���S�������x
	};
	ComPtr<ID3D12Resource> constBuff; // �萔�o�b�t�@
	float destruction;
	float scaleFactor; //�X�P�[���̕ω���
	float positionFactor;//�|�W�V�����̕ω���
	float rotationFactor;//��]�̕ω���
	int  tessellation;//�|���S�������x

	//���S�A�j���[�V�����t���O
	bool playBreakAnimation = false;
	//���S�A�j���[�V�����̑��x
	const float breakSpeed = 0.05f;

	//�_���[�W�t���O
	bool isDamage = false;
	int hp;
	const int MaxHP = 100;
	//������q�b�g��h���J�E���^�[
	int damageCounter;
	//�U�����󂯂Ă���̖��G����(�󂯂��Z���Ƃɕς��邽�ߕϐ�)
	int invTime = 0;

#ifdef _DEBUG
	Object* hitBox = nullptr;
#endif

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



