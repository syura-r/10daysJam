#pragma once

#include "CollisionPrimitive.h"
#include "DebugCamera.h"
#include "FbxLoader.h"
#include "Object.h"
#include "QueryCallback.h"
#include "Sprite.h"

class Player :
	public Object
{
public:
	//攻撃状態の種類
	enum ATTACKSTATE
	{
		NoAttack,//攻撃してない
		MeleeAttack,//近接攻撃
		JumpAttack,//空中攻撃
		Boomerang,//ブーメラン
		ULT//左斬り
	};

public:
	Player();
	~Player();
	void Initialize()override;
	void Update()override;
	void OnCollision(const CollisionInfo& info)override;
	void Draw() override;
	bool GetWaitFight() { return waitFight; }
	static void SetDebugCamera(DebugCamera* cameraPtr) { camera = cameraPtr; }
	static void SetLightCamera(LightCamera* cameraPtr) { lightCamera = cameraPtr; }
	const ATTACKSTATE& GetAttackState() { return nowAttackState; }
	const bool& GetStartFight() { return startFight; }
	inline void StartFight()
	{
		startFight = true;
		notMove = false;
		waitFight = false;
	}
private://メンバ関数
	//攻撃処理
	void Attack();
	//移動処理
	void Move();

private://メンバ変数
	//接地フラグ
	bool onGround = true;
	//落下ベクトル
	XMVECTOR fallV;
	//移動速度
	float speed = 0.1f;
	Vector3 prePos;
	//ナのオブジェクト
	Object* naObject = nullptr;
	Vector3 naPos;
	//進行方向(右向きかどうか)
	bool moveRight;
	//進行方向が変わった時にペラペラさせるフラグ
	bool moveShift;

	//現在の攻撃状態
	ATTACKSTATE nowAttackState;

	//アニメーションの種類
	enum ANIMATIONSTATE
	{
		Wait,//待機アニメーション
		Walk,//歩き
		Jump,//ジャンプ
		Turn,//振り向き
		Attacks,//攻撃
	};
	//現在のアニメーション状態
	ANIMATIONSTATE nowAnimationState;

	//攻撃フラグ
	bool attackFrag;

	//攻撃判定用のカウンタ
	int attackCounter;

	int hp;

	//複数回ダメージを撃受けるのを防ぐフラグ
	bool damage;
	//無敵時間
	int invCounter;


	//HP用のスプライト
	Sprite* hpSprites[4];

//-------------------ボス演出用--------------------
	bool notMove;//戦闘開始前の自動移動フラグ
	bool startFight;//ボス戦開始フラグ
	bool waitFight;//アニメーション終了後ボスの出現待ちフラグ
public:
	const float WallLeft = 40;//左側のカベ
	const float WallRight = 60;//右側のカベ
private:
	bool drawBlackTex;//帯を出すフラグ
	Sprite* upTex;//上部に出てくる黒い帯
	Sprite* downTex;//下部に出てくる黒い帯
	float texSizeY;//上下の帯の縦サイズ
	int texSizeCounter;//イージング用のカウンター
//-------------------------------------------------	
//-------------------デバッグ用-------------------
#ifdef _DEBUG
	Object* hitObj = nullptr;
	Vector3 rejectVal = {}; 
#endif _DEBUG
//------------------------------------------------

//---------------ブーメラン------------------
		//ブーメランの発射方向
	bool boomerangMoveRight;
	//ブーメランの最大移動速度
	const float BoomerangMaxSpeed = 0.3f;
	float  boomerangSpeed;
	//ブーメランの減速までの時間
	const int BoomerangTime = 15;
	//ブーメラン減速までのカウンター
	int boomerangCounter;

	bool boomerang;
//-------------------------------------------

//------------攻撃アニメーション用-----------
	Object* itiObject = nullptr;//左の一角目(一の部分)
	Object* noObject = nullptr;//左の二角目(ノの部分)
	Object* eObject = nullptr;//左の残り(エの部分)
	Object* zanzoObject = nullptr;//残像

	//残像エフェクトの表フラグ
	bool drawZanzoFrag;
	
	//近接段階
	int meleeAttackStage;
	//2段目以降継続させるかのフラグ
	bool continueMeleeAttack;


	Object* hidariGiri[5] = {};

	bool drawHidariGiri[5];
	FbxTime drawFrame[5];

//-------------------------------------------
private://内部クラス
	//クエリーコールバッククラス
	class PlayerQueryCallBack :public QueryCallback
	{
	public:
		PlayerQueryCallBack(Box* box) :box(box) {};
		PlayerQueryCallBack(Sphere* sphere) :sphere(sphere) {};
		//衝突時のコールバック関数
		bool OnQueryHit(const QueryHit& info)
		{
			//ワールドの上方向
			const XMVECTOR up = { 0,1,0,0 };
			//排斥方向
			XMVECTOR rejectDir = XMVector3Normalize(info.reject);
			//上方向と排斥方向の角度差のコサイン値
			float cos = XMVector3Dot(rejectDir, up).m128_f32[0];

			//地面判定しきい値角度
			const float threshold = cosf(XMConvertToRadians(30.0f));
			//角度差によって天井または地面と判定される場合を除いて
			//if (-threshold < cos && cos < threshold)
			//{
				///球を排斥(押し出す)
			if (box)
				box->center += info.reject;
			if (sphere)
				sphere->center += info.reject;
			move += info.reject;

			//}
			return true;
		}
		//クエリーに使用する球
		Box* box = nullptr;
		Sphere* sphere = nullptr;

		//排斥による移動量(累積値)
		DirectX::XMVECTOR move = {};
	};

private://静的メンバ変数
	static DebugCamera* camera;
	static LightCamera* lightCamera;

};

