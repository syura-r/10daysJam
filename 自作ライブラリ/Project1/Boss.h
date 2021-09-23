#pragma once
#include "CollisionPrimitive.h"
#include "Object.h"
#include "QueryCallback.h"
#include "Sprite.h"

class Player;
class Boss :
	public Object
{
public:
	Boss(const Vector3& pos);
	~Boss();
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnCollision(const CollisionInfo& info) override;

	static void SetPlayer(Player* playerPtr) { Boss::player = playerPtr; }
private:
	void NextMovePoint();
	void CauseAttack();//攻撃発生処理
	void Move();
	void Attack();
	void CreateConstBuff();
	void RollingMove();
	struct ConstBuffData
	{
		float _Destruction; //分解度合い
		float _ScaleFactor; //スケールの変化量
		float _PositionFactor;//ポジションの変化量
		float _RotationFactor;//回転の変化量
		int _Tessellation;//ポリゴン分割度
		int _OnEasing;//イージングで分解するか
	};

	//進行方向
	enum RollingDirection
	{
		LEFT,
		RIGHT,
		UP,
		DOWN
	};
	RollingDirection nowRollingDirection;
	//ボスの状態
	enum State
	{
		WAIT,//何もしてない
		MOVE,//攻撃前後の移動
		ATTACK,//攻撃
	};
	Sprite* hpTex[3];
	// 現在の状態
	State nowState;
	//下向き加速
	const float fallAcc = -0.02f;
	const float fallVYMin = -0.5f;

	//回転後倒れるときの速度
	Vector3 slipRotVel = {};
	
	//攻撃の種類
	enum AttackType
	{
		Rocket,
		RollingRush,
		SideRush,
		BirthChildren,
		NOAttack,
	};
	//今の攻撃の種類
	AttackType attackType;
	//初期位置
	Vector3 BasePos;
	//揺れているときのどこで加速度を変えるか示す位置
	Vector2 AfterPos;
	//攻撃タイミングを計るカウンター
	int attackCounter;
	//攻撃のタイミングを示すフラグ
	bool attack;
	//攻撃前後で移動する時の目的地
	Vector3 destination;
	//攻撃前後で移動する時の速度
	Vector3 toDestinationVel;
	//最大回転速度
	const float RotMaxVel = 20.0f;
	//回転速度
	float rotVel;
	//落下速度
	float fallV = 0;
	struct Alert
	{
		//警告用のスプライト
		Sprite* alertTex = nullptr;
		//警告フラグ
		bool drawAlert;
		//警告スプライトの描画位置
		Vector2 pos;
		float alpha;
		//点滅用のカウンター
		int counter;
	};
	Alert alert;
	
	//生成位置
	Vector3 InitPos;

	bool end;

	Sprite* flashTex = nullptr;
	bool drawFlash;
	float flashAlpha;
	int flashCounter;
	
	//死亡アニメーションフラグ
	bool playBreakAnimation = false;
	//死亡アニメーションの速度
	const float breakSpeed = 0.01f;
	
	//ダメージフラグ
	bool isDamage = false;
	int hp;
	const int MaxHP = 2000;
	//複数回ヒットを防ぐカウンター
	int damageCounter;
	//攻撃を受けてからの無敵時間(受けた技ごとに変えるため変数)
	int invTime = 0;

	//出現アニメーションの速度
	const float createSpeed = 0.004f;

	//出現アニメーションの開始フラグ
	bool appearAnimation;
	//出現状態
	bool appear;
	//出現後白く光らせるためのカウンター
	int whiteCounter;
	
	//速度
	float speed = 0.02f;

	//1角目と2角目のオブジェクト
	Object* naObject = nullptr;
	Vector3 naPos;

	//ロケット攻撃時の当たり判定を視覚化するためのオブジェクト
	Object* lockOnObj = nullptr;

	//待機時に移動するためのタイマー
	int moveCounter;

	//攻撃を順番はランダムに全通り出すためのフラグ
	bool selectAttack[4];
	
#ifdef _DEBUG
	Object* hitBox = nullptr;
#endif
	ComPtr<ID3D12Resource> constBuff; // 定数バッファ
	float destruction;
	float scaleFactor; //スケールの変化量
	float positionFactor;//ポジションの変化量
	float rotationFactor;//回転の変化量
	int  tessellation;//ポリゴン分割度
	bool  onEasing;//ポリゴン分割度


	//bool onGround = false;
	//XMVECTOR fallV = {};

	//クエリーコールバッククラス
	class BossQueryCallBack :public QueryCallback
	{
	public:
		BossQueryCallBack(Box* box) :box(box) {};
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

private://静的メンバ変数
	//プレイヤーのポインタ
	static Player* player;
};



