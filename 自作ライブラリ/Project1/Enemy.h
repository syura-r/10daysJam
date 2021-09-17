#pragma once
#include "Object.h"
class Enemy :
	public Object
{
public:
	Enemy(const Vector3& pos);
	void Update() override;
	void Draw() override;
	bool IsDelete() { return isDelete; }
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

	//���S�t���O
	bool isDead = false;
	//���S�A�j���[�V�����I���t���O
	bool isDelete = false;

	const float breakSpeed = 0.05f;
};



