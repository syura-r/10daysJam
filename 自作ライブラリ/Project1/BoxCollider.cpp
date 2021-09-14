#include "BoxCollider.h"

void BoxCollider::Update()
{
	const XMMATRIX& matWorld = object->GetMatWorld();

	Box::center = matWorld.r[3] + offset;
	Box::scale = scale;
	Box::minPosition = matWorld.r[3] + offset - scale;
	Box::maxPosition = matWorld.r[3] + offset + scale;
}

const Vector3 BoxCollider::GetMax()
{
	return maxPosition;
}

const Vector3 BoxCollider::GetMin()
{
	return minPosition;
}
