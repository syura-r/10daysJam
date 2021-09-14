#pragma once
#include "Object.h"
class GSShaderTestObj :
    public Object
{
public:
	GSShaderTestObj(const Vector3& pos);
	void Draw() override;
private:
	void CreateConstBuff();
	struct ConstBuffData
	{
		//XMFLOAT4 status;
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
};

