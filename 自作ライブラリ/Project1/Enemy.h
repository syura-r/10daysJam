#pragma once
#include "Object.h"

class Player;
class Enemy :
	public Object
{
public:
	Enemy(const Vector3& pos);
	~Enemy();
	void Update() override;
	void Draw() override;
	void OnCollision(const CollisionInfo& info) override;

	static void SetPlayer(Player* playerPtr) { Enemy::player = playerPtr; }
private:
	void CreateConstBuff();
	struct ConstBuffData
	{
		float _Destruction; //分解度合い
		float _ScaleFactor; //スケールの変化量
		float _PositionFactor;//ポジションの変化量
		float _RotationFactor;//回転の変化量
		int _Tessellation;//ポリゴン分割度
	};
	ComPtr<ID3D12Resource> constBuff; // 定数バッファ
	float destruction;
	float scaleFactor; //スケールの変化量
	float positionFactor;//ポジションの変化量
	float rotationFactor;//回転の変化量
	int  tessellation;//ポリゴン分割度

	//死亡アニメーションフラグ
	bool playBreakAnimation = false;
	//死亡アニメーションの速度
	const float breakSpeed = 0.05f;

	//ダメージフラグ
	bool isDamage = false;
	int hp;
	const int MaxHP = 100;
	//複数回ヒットを防ぐカウンター
	int damageCounter;

	
	Object* hitBox = nullptr;
private://静的メンバ変数
	//プレイヤーのポインタ
	static Player* player;
};



