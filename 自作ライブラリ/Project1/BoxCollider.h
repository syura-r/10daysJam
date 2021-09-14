#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"

class BoxCollider : public BaseCollider ,public Box
{
public:
	BoxCollider(XMVECTOR offset = { 0,0,0,0 },Vector3 scale ={1,1,1}) :
		offset(offset),scale(scale)
	{
		shapeType = COLLISIONSHAPE_BOX;
	}

	void Update()override;
	const Vector3 GetMax() override;
	const Vector3 GetMin() override;

	inline void SetScale(const Vector3& scale) { this->scale = scale; }

	inline const XMVECTOR& GetOffset() { return offset; }

	inline void SetOffset(const XMVECTOR& offset) { this->offset = offset; }

	inline const Vector3& GetScale() { return scale; }
private:
	XMVECTOR offset;
	Vector3 scale;

};

