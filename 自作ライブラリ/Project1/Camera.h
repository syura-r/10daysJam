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
	XMMATRIX matView;//ビュー変換行列
	float angle = 0.0f;
	XMFLOAT3 eye;  //視点座標
	Vector3 target;  //視点座標
	XMFLOAT3 up;  //視点座標
	// ビュー行列ダーティフラグ
	bool viewDirty = false;
	//ビルボード行列
	XMMATRIX matBillboard = DirectX::XMMatrixIdentity();
	// Y軸回りビルボード行列
	XMMATRIX matBillboardY = DirectX::XMMatrixIdentity();
	// 射影行列
	XMMATRIX matProjection = DirectX::XMMatrixIdentity();
	// ビュー射影行列
	XMMATRIX matViewProjection = DirectX::XMMatrixIdentity();

};

