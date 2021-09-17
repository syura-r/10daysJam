#pragma once
#include "DebugCamera.h"
#include "Object.h"
class Player :
	public Object
{
public:
	Player();
	~Player();
	void Initialize()override;
	void Update()override;
	void OnCollision(const CollisionInfo& info)override;
	void Draw() override;
	static void SetDebugCamera(DebugCamera* cameraPtr) { camera = cameraPtr; }
	static void SetLightCamera(LightCamera* cameraPtr) { lightCamera = cameraPtr; }

private:
	//攻撃処理
	void Attack();
	//移動処理
	void Move();
	//接地フラグ
	bool onGround = true;
	//落下ベクトル
	XMVECTOR fallV;
	//移動速度
	float speed = 0.3f;
	Vector3 prePos;
	//ナのオブジェクト
	Object* naObject = nullptr;
	//進行方向(右向きかどうか)
	bool moveRight;
	//進行方向が変わった時にペラペラさせるフラグ
	bool moveShift;
	//攻撃状態の種類
	enum ATTACKSTATE
	{
		NONE,//攻撃してない
		MeleeAttack,//近接攻撃
		JumpAttack,//空中攻撃
		Boomerang,//ブーメラン
		ULT//左斬り
	};
	//現在の攻撃状態
	ATTACKSTATE nowAttackState;

	enum ANIMATIONSTATE
	{
		Wait,//待機アニメーション
		Walk,//歩き
		Jump,//ジャンプ
		Turn,//振り向き
	};
	//現在のアニメーション状態
	ANIMATIONSTATE nowAnimationState;
	
	Object* hitObj = nullptr;
private://静的メンバ変数
	static DebugCamera* camera;
	static LightCamera* lightCamera;

};

