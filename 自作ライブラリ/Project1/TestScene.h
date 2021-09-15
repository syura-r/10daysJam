#pragma once
#include "Scene.h"
#include"LightGroup.h"
#include "Object.h"
#include "OBJLoader.h"
#include "SmartPtr.h"
#include "Vector.h"
#include"Tree.h"
#include<vector>
class SPHERE:public Object
{
public:
	DWORD ID;
	Vector3 p;			// 球の位置
	Vector3 Pre_p;		// 1つ前の円の位置
	Vector3 v;			// 速度ベクトル
	Vector3 a;			// 加速度ベクトル
	float r;				// 半径
	float w;				// 質量
	float scale;			// スケール

	SPHERE()
	{
		Create(OBJLoader::GetModel("sphere"));
		p.x = p.y = p.z = 0.0f;
		Pre_p.x = Pre_p.y = Pre_p.z = 0.0f;
		v.x = v.y = v.z = 0.0f;
		a.x = a.y = a.z = 0.0f;
		r = 1.0f;
		w = 1.0f;
		scale = 1.0f;
	}
};


class TestScene :
    public Scene
{
public:
	TestScene();
	~TestScene()override;
	void Initialize()override;
	void Update()override;
	void PreDraw()override;
	void PostDraw()override;
private:
	LightGroup* lightGroup = nullptr;
	float coloramb[3] = { 1,1,1 };
	float color0[3] = { 1,1,1 };
	float color1[3] = { 1,1,1 };
	float color2[3] = { 1,1,1 };
	float lightDir[3] = { 0,0,1 };

	std::vector<SPHERE*> ColVect;	// 衝突対象リスト

	
	DWORD collisionCount;
	static const int g_SphereNum = 1000;

	float g_Circle_Ref = 0.75f;	// 球同士の反発係数
	float Wall_Ref = 0.78f;		// 球と壁の反発係数
	int g_PartitionLevel = 6;		// 空間分割レベル
	float g_Gravity = 9.8f;			// 重力
	bool g_XLeft = true;	// X軸左壁
	bool g_XRight = true;	// X軸右壁
	bool g_ZFar = true;	// Z軸奥壁
	bool g_ZNear = true;	// Z軸手前壁
	bool g_WindowMode = false;	// ウィンドウモード/フルスクリーン切り替え

	int cn;



	SPHERE SAry[g_SphereNum];
	SmartPtr<Tree::TreeObject<SPHERE> > spSOFTAry[g_SphereNum];	// 球オブジェクトを包むOFTオブジェクト
	Tree::CLiner8TreeManager<SPHERE> L8Tree;

	Object* wall[5];
	bool drawWall = true;
//////////////////////////////////////////////////////////	
	void SphereColProc(SPHERE* s1, SPHERE* s2);

	// 次の球の位置を取得
	void GetNextSpherePos(SPHERE& s)
	{
		Vector3 RefV;	// 反射後の速度ベクトル
		Vector3 ColliPos;	// 衝突位置
		float Res_time = 0.0f;	// 衝突後の移動可能時間

		// 重力を掛けて落とす
		s.v.y -= g_Gravity / 60;	// 1フレームで9.8/60(m/s)加速

		// 今の速度で位置を更新
		s.Pre_p = s.p;		// 前の位置を保存
		s.p += s.v;			// 位置更新

		// 壁との衝突をチェック
		// X左壁
		if (s.p.x < s.r && g_XLeft) {
			// 反射後の速度ベクトルを取得
			GetRefrectVelo(&RefV, Vector3(1, 0, 0), s.v, Wall_Ref);
			// 残り時間算出
			Res_time = (s.p.x - s.r) / s.v.x;
			// 反射後の位置を算出
			GetRelectedPos(Res_time, s, RefV);
		}
		// X右壁
		else if (s.p.x > 640 - s.r && g_XRight) {
			GetRefrectVelo(&RefV, Vector3(-1, 0, 0), s.v, Wall_Ref);
			Res_time = (s.p.x - 640 + s.r) / s.v.x;
			GetRelectedPos(Res_time, s, RefV);
		}
		// Z手前壁
		if (s.p.z < s.r && g_ZNear) {
			GetRefrectVelo(&RefV, Vector3(0, 0, 1), s.v, Wall_Ref);
			Res_time = (s.p.z - s.r) / s.v.z;
			GetRelectedPos(Res_time, s, RefV);
		}
		// Z奥壁
		else if (s.p.z > 640 - s.r && g_ZFar) {
			GetRefrectVelo(&RefV, Vector3(0, 0, -1), s.v, Wall_Ref);
			Res_time = (s.p.z - 640 + s.r) / s.v.z;
			GetRelectedPos(Res_time, s, RefV);
		}
		// Y下壁
		else if (s.p.y < s.r) {
			GetRefrectVelo(&RefV, Vector3(0, 1, 0), s.v, Wall_Ref);
			Res_time = (s.p.y - s.r) / s.v.y;
			GetRelectedPos(Res_time, s, RefV);
		}
	}
	// 壁と球の反射ベクトルを計算
	void GetRefrectVelo(Vector3* pOut, Vector3& N, Vector3& V, float e)
	{
		N = N.Normalize();
		*pOut = V - (1 + e) * Vector3::Dot(N, V) * N;
	}
	// 壁との反射後の位置を算出
	void GetRelectedPos(float Res_time, SPHERE& s, Vector3& RefV)
	{
		// 衝突位置
		// 0.99は壁にめり込まないための補正
		s.p = s.Pre_p + s.v * (1 - Res_time) * 0.99f;
		// 反射ベクトル
		s.v = RefV;
		// 位置を補正
		s.p += s.v * Res_time;
	}

	
	bool CheckParticleCollision(
		FLOAT rA, FLOAT rB,
		Vector3* pPre_pos_A, Vector3* pPos_A,
		Vector3* pPre_pos_B, Vector3* pPos_B,
		FLOAT* pOut_t,
		Vector3* pOut_colli_A,
		Vector3* pOut_colli_B
	)
	{
		// 前位置及び到達位置におけるパーティクル間のベクトルを算出
		Vector3 C0 = *pPre_pos_B - *pPre_pos_A;
		Vector3 C1 = *pPos_B - *pPos_A;
		Vector3 D = C1 - C0;

		// 衝突判定用の2次関数係数の算出
		FLOAT P = D.Length() * D.Length(); if (P <= 0) return false; // 同じ方向に移動
		FLOAT Q = Vector3::Dot(C0, D); if (Q == 0) return false; // 平行
		FLOAT R = C0.Length() * C0.Length();

		// パーティクル距離
		FLOAT r = rA + rB;

		// 衝突判定式
		FLOAT Judge = Q * Q - P * (R - r * r);
		if (Judge < 0) {
			// 衝突していない
			return false;
		}

		// 衝突時間の算出
		FLOAT t_plus = (-Q + sqrt(Judge)) / P;
		FLOAT t_minus = (-Q - sqrt(Judge)) / P;

		// 衝突時間が0未満1より大きい場合、衝突しない
	 //   if( (t_plus < 0 || t_plus > 1) && (t_minus < 0 || t_minus > 1)) return false;
		if (t_minus < 0 || t_minus > 1) return false;

		// 衝突時間の決定（t_minus側が常に最初の衝突）
		*pOut_t = t_minus;

		// 衝突位置の決定
		*pOut_colli_A = *pPre_pos_A + t_minus * (*pPos_A - *pPre_pos_A);
		*pOut_colli_B = *pPre_pos_B + t_minus * (*pPos_B - *pPre_pos_B);

		return true; // 衝突報告
	}




	///////////////////////////////////////////////////
	// パーティクル衝突後速度位置算出関数
	//   pColliPos_A : 衝突中のパーティクルAの中心位置
	//   pVelo_A     : 衝突の瞬間のパーティクルAの速度
	//   pColliPos_B : 衝突中のパーティクルBの中心位置
	//   pVelo_B     : 衝突の瞬間のパーティクルBの速度
	//   weight_A    : パーティクルAの質量
	//   weight_B    : パーティクルBの質量
	//   res_A       : パーティクルAの反発率
	//   res_B       : パーティクルBの反発率
	//   time        : 反射後の移動可能時間
	//   pOut_pos_A  : パーティクルAの反射後位置
	//   pOut_velo_A : パーティクルAの反射後速度ベクトル
	//   pOut_pos_B  : パーティクルBの反射後位置
	//   pOut_velo_B : パーティクルBの反射後速度ベクトル
	bool CalcParticleColliAfterPos(
		Vector3* pColliPos_A, Vector3* pVelo_A,
		Vector3* pColliPos_B, Vector3* pVelo_B,
		FLOAT weight_A, FLOAT weight_B,
		FLOAT res_A, FLOAT res_B,
		FLOAT time,
		Vector3* pOut_pos_A, Vector3* pOut_velo_A,
		Vector3* pOut_pos_B, Vector3* pOut_velo_B
	)
	{
		FLOAT TotalWeight = weight_A + weight_B; // 質量の合計
		FLOAT RefRate = (1 + res_A * res_B); // 反発率
		Vector3 C = *pColliPos_B - *pColliPos_A; // 衝突軸ベクトル
		C.Normalize();
		FLOAT Dot = Vector3::Dot((*pVelo_A - *pVelo_B), C); // 内積算出
		Vector3 ConstVec = RefRate * Dot / TotalWeight * C; // 定数ベクトル

		// 衝突後速度ベクトルの算出
		*pOut_velo_A = -weight_B * ConstVec + *pVelo_A;
		*pOut_velo_B = weight_A * ConstVec + *pVelo_B;

		// 衝突後位置の算出
		*pOut_pos_A = *pColliPos_A + time * (*pOut_velo_A);
		*pOut_pos_B = *pColliPos_B + time * (*pOut_velo_B);

		return true;
	}


	///////////////////////////////////////////////////
	// 平面パーティクル衝突判定・時刻・位置算出関数
	//   r          : パーティクルの半径
	//   pPre_pos   : パーティクルの前の位置
	//   pPos       : パーティクルの次の到達位置
	//   pNormal    : 平面の法線
	//   pPlane_pos : 平面上の1点
	//   pOut_t     : 衝突時間を格納するFLOAT型へのポインタ
	//   pOut_colli : パーティクルの衝突位置を格納するD3DXVECTOR型へのポインタ

	bool CheckParticlePlaneCollision(
		FLOAT r,
		Vector3* pPre_pos, Vector3* pPos,
		Vector3* pNormal, Vector3* pPlane_pos,
		FLOAT* t,
		Vector3* pOut_colli
	)
	{
		Vector3 C0 = *pPre_pos - *pPlane_pos; // 平面上の一点から現在位置へのベクトル
		Vector3 D = *pPos - *pPre_pos; // 現在位置から予定位置までのベクトル
		Vector3 N = Vector3::Normalize(*pNormal);

		// 衝突判定
		FLOAT Dot = Vector3::Dot(D, N);
		if (Dot >= 0)
			return false; // 交差しない

		 // 交差時間の算出
		FLOAT Dot_C0 = Vector3::Dot(C0, N);
		*t = (r - Dot_C0) / Dot;

		// 衝突位置の算出
		*pOut_colli = *pPre_pos + (*t) * D;

		// 衝突判定
		// 交差時間が0～1の間ならば衝突
		if (*t >= 0 && *t <= 1)
			return true; // 衝突報告

		 // 壁にめり込んでいる状態であれば
		 // 壁の前に押し戻す
		if (-Dot < r) {
			*pOut_colli = *pPre_pos - Dot * N;
		}

		return false;
	}


	///////////////////////////////////////////////////
	// 平面と球の衝突後速度算出関数
	// pColliPos : 衝突中のパーティクルの中心位置
	// pVelo : 衝突の瞬間のパーティクルの速度
	// res : パーティクルの壁に対する反発率
	// time : 反射後の移動可能時間
	// pNormal : 平面の法線
	// pOut_pos : パーティクルの反射後位置
	// pOut_velo : パーティクルの反射後速度ベクトル

	bool CalcParticlePlaneAfterPos(
		Vector3* pColliPos,
		Vector3* pVelo,
		FLOAT res,
		FLOAT time,
		Vector3* pNormal,
		Vector3* pOut_pos,
		Vector3* pOut_velo
	)
	{
		// 反射後速度を算出
		Vector3 N = Vector3::Normalize(*pNormal);
		*pOut_velo = *pVelo - (1 + res) * Vector3::Dot(N, *pVelo) * N;

		// 移動位置を計算
		*pOut_pos = *pColliPos + *pOut_velo * time;

		return true;
	}


	
};

