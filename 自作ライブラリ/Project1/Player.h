#pragma once
#include "DebugCamera.h"
#include "Object.h"
class Player :
	public Object
{
public:
	Player();
	void Initialize()override;
	void Update()override;
	void OnCollision(const CollisionInfo& info)override;
	void Draw() override;
	static void SetDebugCamera(DebugCamera* cameraPtr) { camera = cameraPtr; }
	static void SetLightCamera(LightCamera* cameraPtr) { lightCamera = cameraPtr; }

private:
	//�ړ�����
	void Move();
	//�ڒn�t���O
	bool onGround = true;
	//�����x�N�g��
	XMVECTOR fallV;
	//���݌����Ă����
	Vector3 direction;
	//�ړ����x
	float speed = 0.3f;
	//��]���x
	float rotateSpeed = 7.0f;
	Vector3 prePos;
	//�J������]��
	bool rotCamera;
	//�J�����̉�]�x����
	float radY;
	int cameraRotCount;
private://�ÓI�����o�ϐ�
	static DebugCamera* camera;
	static LightCamera* lightCamera;

};

