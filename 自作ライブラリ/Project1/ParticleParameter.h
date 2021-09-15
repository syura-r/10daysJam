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
	//ズレ
	Vector3 gap = {};
	// 座標
	Vector3 position = {};
	// 速度
	Vector3 velocity = {};
	// 加速度
	Vector3 accel = {};
	// 色
	Vector3 color = {};
	//透明度
	float alpha = 1.0f;
	// スケール
	float scale = 1.0f;
	float scaleVel = 0.0f;
	float scaleAce = 0.0f;
	// 回転
	Vector3 rotation = { 0,0,0 };
	// 初期値
	Vector3 s_color = {};
	float s_scale = 1.0f;
	Vector3 s_rotation = { 0,0,0 };
	float s_alpha = 1.0f;
	// 最終値
	Vector3 e_color = {};
	float e_scale = 0.0f;
	Vector3 e_rotation = { 0,0,0 };
	float e_alpha = 0.0f;
	// 現在フレーム
	int frame = 0;
	// 終了フレーム
	int num_frame = 0;
	//死亡フラグ
	bool isDead = false;
	//ビルボードするかどうか
	bool billboradActive = true;
	//画像名
	//std::string texName;
	//目標オブジェクト
	Object* targetObj = nullptr;
	Player* player = nullptr;
	//目標位置
	Vector3 startPos = {};
	Vector3 endPos = {};
	int maxFrame = 0;
	int easeFrame = 0;
	ParticleType type = DEFAULT;

	BLENDTYPE blendType = ADD;
};