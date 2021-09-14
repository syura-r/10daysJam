#include "DebugCamera.h"

#include <cstdio>

#include "Input.h"
#include "Vector.h"
#include "window.h"

DebugCamera::DebugCamera()
{
	phi = -3.14159265f/2;
	theta = 0;
}

void DebugCamera::Update()
{
	bool dirty = false;
	//float angleX = 0;
	//float angleY = 0;

	auto mouseMove = Input::GetMouseMove();

	// �}�E�X�̍��{�^����������Ă�����J��������]������
	if(Input::DownMouseKey(Input::RIGHT))
	{

		phi += XM_PI/180.0f * mouseMove.lX / 7;
		theta += XM_PI / 180.0f * mouseMove.lY /7;
		if (theta > 40 * XM_PI / 180.0f)
			theta = 40 * XM_PI / 180.0f;
		else if (theta < -40 * XM_PI / 180.0f)
			theta = -40 * XM_PI / 180.0f;

		if (phi > 360 * XM_PI / 180.0f)
			phi -= 360 * XM_PI / 180.0f;
		else if (phi < 0)
			phi += 360 * XM_PI / 180.0f;
		
		if(theta < 0)
		{
			distance = 10 * (1 + theta * 1.1f);
			
		}
		//printf("phi : %f , theta : %f\n", phi, theta);
		dirty = true;
	}

	// �}�E�X�̒��{�^����������Ă�����J��������s�ړ�������
	if (Input::DownMouseKey(Input::MIDDLE))
	{
		XMMATRIX camMatWorld = XMMatrixInverse(nullptr, GetMatView());
		auto dy = camMatWorld.r[1] * mouseMove.lY / 10;
		auto dx = camMatWorld.r[0] * mouseMove.lX / 10;

		
		target += Vector3(dx.m128_f32[0], dx.m128_f32[1], dx.m128_f32[2]);
		target += Vector3(dy.m128_f32[0], dy.m128_f32[1], dy.m128_f32[2]);

		dirty = true;
	}

	//R�������ꂽ�烊�Z�b�g
	if(Input::TriggerKey(DIK_R))
	{
		phi = 3.14159265f / 2;
		theta = 0;
		eye = XMFLOAT3(0, 0, -10);
		target = XMFLOAT3(0, 0, 0);
		up = XMFLOAT3(0, 1, 0);
		distance = 10;
		viewDirty = true;
		Camera::Update();
		return;
	}

	// �z�C�[�����͂ŋ�����ύX
	if (mouseMove.lZ != 0) {
		distance -= mouseMove.lZ / 100.0f;
		distance = max(distance, 1.0f);
		dirty = true;
	}

	if (dirty || viewDirty) {
		float nowTheta = theta;
		if (theta < 0)
			nowTheta = 0;
		eye = Vector3( cos(phi)*cos(nowTheta), sin(nowTheta) ,sin(phi)*cos(nowTheta)) * distance + target;
		viewDirty = true;

	}

	Camera::Update();

}
