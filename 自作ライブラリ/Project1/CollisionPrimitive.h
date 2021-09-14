#pragma once
#include<DirectXMath.h>
#include"Vector.h"
struct Ray
{
	// 始点
	DirectX::XMVECTOR	start = { 0,0,0,1 };
	// 方向
	DirectX::XMVECTOR	dir = { 1,0,0,0 };
};

struct Plane
{
	// 法線ベクトル
	DirectX::XMVECTOR normal = { 0,1,0 };
	// 原点からの距離
	float distance = 0.0f;
};

struct Sphere
{
	// 中心座標
	DirectX::XMVECTOR center = {};
	// 半径
	float radius = 1.0f;
};

struct Box
{
	//中心座標
	DirectX::XMVECTOR center = {};
	//大きさ
	Vector3 scale = { 1,1,1 };
	Vector3 minPosition = {};
	Vector3 maxPosition = {};

};

class Triangle
{
public:
	// 頂点座標3つ
	DirectX::XMVECTOR	p0;
	DirectX::XMVECTOR	p1;
	DirectX::XMVECTOR	p2;
	// 法線ベクトル
	DirectX::XMVECTOR	normal;

	/// <summary>
	/// 法線の計算
	/// </summary>
	void ComputeNormal();
};
