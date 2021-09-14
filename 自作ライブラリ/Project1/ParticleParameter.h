#pragma once
#include <intsafe.h>

#include"Vector.h"
#include<string>
struct ParticleParameter
{
	// �����x
	Vector3 accel = {0,0,0};
	// �X�P�[��
	float scaleAce = 0;
	// �����l
	Vector3 s_color = {0,0,0};
	float s_scale = 1;
	Vector3 s_rotation;
	float s_alpha = 1;
	// �ŏI�l
	Vector3 e_color = {1,1,1};
	float e_scale = 1;
	Vector3 e_rotation = {0,0,0};
	float e_alpha  = 1;
	// �I���t���[��
	int num_frame = 60;
	//Vector3 pad = {0,0,0};
	//// ���W
	//Vector3 position = {};
	//// ���x
	//Vector3 velocity = {};
	//// �����x
	//Vector3 accel = {};
	//// �F
	//Vector3 color = {};
	////�����x
	//float alpha = 1.0f;
	//// �X�P�[��
	//float scale = 1.0f;
	//float scaleVel = 0.0f;
	//float scaleAce = 0.0f;
	//// ��]
	//Vector3 rotation = {0,0,0};
	//// �����l
	//Vector3 s_color = {};
	//float s_scale = 1.0f;
	//Vector3 s_rotation = { 0,0,0 };
	//float s_alpha = 1.0f;
	//// �ŏI�l
	//Vector3 e_color = {};
	//float e_scale = 0.0f;
	//Vector3 e_rotation = { 0,0,0 };
	//float e_alpha = 0.0f;
	//// ���݃t���[��
	//int frame = 0;
	//// �I���t���[��
	//int num_frame = 0;
	////���S�t���O
	//bool isDead = false;
	////�r���{�[�h���邩�ǂ���
	//bool billboardActive = true;
	//�摜��
	//std::string texName;
};

struct OutputData
{
	Vector3 position;
	float scale;

	Vector3 rotation;
	//�r���{�[�h���邩�ǂ���
	UINT billboardActive;

	DirectX::XMFLOAT4 color;

	Vector3 velocity;
	UINT frame;
	//���S�t���O
	UINT isDead;
	float scaleVel;

	Vector2 pad;

};
