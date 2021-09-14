#pragma once
#include <DirectXMath.h>
#include <d3d12.h>

#include "Vector.h"
using namespace DirectX;

class Camera
{
public:
	Camera();	
	Camera(XMFLOAT3 eye,XMFLOAT3 target,XMFLOAT3 up);
	~Camera();

	virtual void Update();
	void UpdateViewMatrix();
	XMFLOAT3& GetEye() { return eye; }
	Vector3& GetTarget() { return target; }
	XMMATRIX& GetMatView() { return matView; }
	XMMATRIX& GetMatProjection() { return matProjection; }
	XMMATRIX& GetMatViewProjection() { 
		matViewProjection = matView * matProjection;
		return matViewProjection; }
	XMMATRIX& GetMatBillboard() { return matBillboard; }
	XMMATRIX& GetMatBillboardY() { return matBillboardY; }
	void SetMatrixView(const XMFLOAT3& eye, const XMFLOAT3& target, const XMFLOAT3& up);
	void MoveVector(const XMVECTOR& moveeye = { }, const XMVECTOR& movetarget = { }, const XMVECTOR& movrup = { });
	void SetTarget(const XMFLOAT3& target) { this->target = target; viewDirty = true; }
protected:
	XMMATRIX matView;//�r���[�ϊ��s��
	float angle = 0.0f;
	XMFLOAT3 eye;  //���_���W
	Vector3 target;  //���_���W
	XMFLOAT3 up;  //���_���W
	// �r���[�s��_�[�e�B�t���O
	bool viewDirty = false;
	//�r���{�[�h�s��
	XMMATRIX matBillboard = DirectX::XMMatrixIdentity();
	// Y�����r���{�[�h�s��
	XMMATRIX matBillboardY = DirectX::XMMatrixIdentity();
	// �ˉe�s��
	XMMATRIX matProjection = DirectX::XMMatrixIdentity();
	// �r���[�ˉe�s��
	XMMATRIX matViewProjection = DirectX::XMMatrixIdentity();

};

