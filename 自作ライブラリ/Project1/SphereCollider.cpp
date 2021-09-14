#include "SphereCollider.h"
using namespace DirectX;
void SphereCollider::Update()
{
	const XMMATRIX& matWorld = object->GetMatWorld();

	Sphere::center = matWorld.r[3] + offset;
	Sphere::radius = radius;
}

const Vector3 SphereCollider::GetMax()
{
	Vector3 tmpMax = Vector3(Sphere::center.m128_f32[0] + radius, Sphere::center.m128_f32[1] + radius, Sphere::center.m128_f32[2] + radius);
	return tmpMax;
}

const Vector3 SphereCollider::GetMin()
{
	Vector3 tmpMin = Vector3(Sphere::center.m128_f32[0] - radius, Sphere::center.m128_f32[1] - radius, Sphere::center.m128_f32[2] - radius);
	return tmpMin;
}
