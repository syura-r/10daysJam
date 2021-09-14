#pragma once
#include <intsafe.h>

#include"Vector.h"
#include<string>
struct ParticleParameter
{
	// 加速度
	Vector3 accel = {0,0,0};
	// スケール
	float scaleAce = 0;
	// 初期値
	Vector3 s_color = {0,0,0};
	float s_scale = 1;
	Vector3 s_rotation;
	float s_alpha = 1;
	// 最終値
	Vector3 e_color = {1,1,1};
	float e_scale = 1;
	Vector3 e_rotation = {0,0,0};
	float e_alpha  = 1;
	// 終了フレーム
	int num_frame = 60;
	//Vector3 pad = {0,0,0};
	//// 座標
	//Vector3 position = {};
	//// 速度
	//Vector3 velocity = {};
	//// 加速度
	//Vector3 accel = {};
	//// 色
	//Vector3 color = {};
	////透明度
	//float alpha = 1.0f;
	//// スケール
	//float scale = 1.0f;
	//float scaleVel = 0.0f;
	//float scaleAce = 0.0f;
	//// 回転
	//Vector3 rotation = {0,0,0};
	//// 初期値
	//Vector3 s_color = {};
	//float s_scale = 1.0f;
	//Vector3 s_rotation = { 0,0,0 };
	//float s_alpha = 1.0f;
	//// 最終値
	//Vector3 e_color = {};
	//float e_scale = 0.0f;
	//Vector3 e_rotation = { 0,0,0 };
	//float e_alpha = 0.0f;
	//// 現在フレーム
	//int frame = 0;
	//// 終了フレーム
	//int num_frame = 0;
	////死亡フラグ
	//bool isDead = false;
	////ビルボードするかどうか
	//bool billboardActive = true;
	//画像名
	//std::string texName;
};

struct OutputData
{
	Vector3 position;
	float scale;

	Vector3 rotation;
	//ビルボードするかどうか
	UINT billboardActive;

	DirectX::XMFLOAT4 color;

	Vector3 velocity;
	UINT frame;
	//死亡フラグ
	UINT isDead;
	float scaleVel;

	Vector2 pad;

};
