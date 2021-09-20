#pragma once
#include "CollisionPrimitive.h"
#include "Object.h"
#include "QueryCallback.h"

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
	//攻撃を受けてからの無敵時間(受けた技ごとに変えるため変数)
	int invTime = 0;

#ifdef _DEBUG
	Object* hitBox = nullptr;
#endif

	//クエリーコールバッククラス
	class EnemyQueryCallBack :public QueryCallback
	{
	public:
		EnemyQueryCallBack(Box* box) :box(box) {};
		//衝突時のコールバック関数
		bool OnQueryHit(const QueryHit& info)
		{
			//ボックスを排斥(押し出す)
			if (box)
				box->center += info.reject;
			move += info.reject;

			return true;
		}
		//クエリーに使用する球
		Box* box = nullptr;
		//排斥による移動量(累積値)
		XMVECTOR move = {};
	};

	bool onGround = false;
	XMVECTOR fallV = {};


private://静的メンバ変数
	//プレイヤーのポインタ
	static Player* player;
};



