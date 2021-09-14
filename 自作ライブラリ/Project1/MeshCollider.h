#pragma once

#include"BaseCollider.h"
#include"CollisionPrimitive.h"

#include<DirectXMath.h>
#include "OBJModel.h"

class MeshCollider:public BaseCollider
{
public:
	MeshCollider()
	{
		//メッシュ形状をセット
		shapeType = COLLISIONSHAPE_MESH;
		maxPoint = {};
		minPoint = {};
	}

	//三角形の配列を構築する
	void ConstructTriangles(OBJModel* model);

	void Update()override;
	const Vector3 GetMax() override;
	const Vector3 GetMin() override;

	//球との当たり判定
	bool CheckCollisionSphere(const Sphere& sphere, DirectX::XMVECTOR* inter = nullptr,DirectX::XMVECTOR* reject = nullptr);

	//レイとの当たり判定
	bool CheckCollisionRay(const Ray& ray, float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);


private:

	void CheckMaxMinPoint(const XMVECTOR& point);
	std::vector<Triangle> triangles;
	//ワールド行列の逆行列
	DirectX::XMMATRIX invMatWorld;
	////前フレームのワールド行列の逆行列
	//DirectX::XMMATRIX preInvMatWorld;
	//一番値の小さい位置にある頂点
	XMVECTOR minPoint;
	//一番値の大きい位置にある頂点
	XMVECTOR maxPoint;
};