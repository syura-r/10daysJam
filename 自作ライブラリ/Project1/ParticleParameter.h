#pragma once
#include"Vector.h"
#include<string>
#include"Object.h";
enum ParticleType
{
	DEFAULT,
	MOVE,
	HOMING,
	TIMEPLUS,
	FEVER,
};
class Player;
struct ParticleParameter
{
	//�Y��
	Vector3 gap = {};
	// ���W
	Vector3 position = {};
	// ���x
	Vector3 velocity = {};
	// �����x
	Vector3 accel = {};
	// �F
	Vector3 color = {};
	//�����x
	float alpha = 1.0f;
	// �X�P�[��
	float scale = 1.0f;
	float scaleVel = 0.0f;
	float scaleAce = 0.0f;
	// ��]
	Vector3 rotation = { 0,0,0 };
	// �����l
	Vector3 s_color = {};
	float s_scale = 1.0f;
	Vector3 s_rotation = { 0,0,0 };
	float s_alpha = 1.0f;
	// �ŏI�l
	Vector3 e_color = {};
	float e_scale = 0.0f;
	Vector3 e_rotation = { 0,0,0 };
	float e_alpha = 0.0f;
	// ���݃t���[��
	int frame = 0;
	// �I���t���[��
	int num_frame = 0;
	//���S�t���O
	bool isDead = false;
	//�r���{�[�h���邩�ǂ���
	bool billboradActive = true;
	//�摜��
	//std::string texName;
	//�ڕW�I�u�W�F�N�g
	Object* targetObj = nullptr;
	Player* player = nullptr;
	//�ڕW�ʒu
	Vector3 startPos = {};
	Vector3 endPos = {};
	int maxFrame = 0;
	int easeFrame = 0;
	ParticleType type = DEFAULT;

	BLENDTYPE blendType = ADD;
};