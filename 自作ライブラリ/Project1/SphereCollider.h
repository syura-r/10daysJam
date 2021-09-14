#pragma once
#include"BaseCollider.h"
#include"CollisionPrimitive.h"

#include<DirectXMath.h>
class SphereCollider:public BaseCollider,public Sphere
{
private:
	using XMVECTOR = DirectX::XMVECTOR;
public:
	SphereCollider(XMVECTOR offset = { 0,0,0,0 }, float radius = 1.0f) :
		offset(offset), radius(radius)
	{
		shapeType = COLLISIONSHAPE_SPHERE;
	}

	void Update()override;

	const Vector3 GetMax() override;
	const Vector3 GetMin() override;
	inline void SetRadius(float radius) { this->radius = radius; }
	
	inline const XMVECTOR& GetOffset() { return offset; }

	inline void SetOffset(const XMVECTOR& offset) { this->offset = offset; }
	
	inline float GetRadius() { return radius; }
private:
	XMVECTOR offset;
	float radius;
};

