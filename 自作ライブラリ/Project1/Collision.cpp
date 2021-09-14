#include "Collision.h"
Window* Collision::window = nullptr;
using namespace DirectX;

void Collision::ClosestPtPoint2Triangle(const DirectX::XMVECTOR & point, const Triangle & triangle, DirectX::XMVECTOR * closest)
{
	//point��p0�̊O���̒��_�̈�̒��ɂ��邩�ǂ����`�F�b�N
	XMVECTOR p0_p1 = triangle.p1 - triangle.p0;
	XMVECTOR p0_p2 = triangle.p2 - triangle.p0;
	XMVECTOR p0_pt = point - triangle.p0;

	float d1 = XMVector3Dot(p0_p1, p0_pt).m128_f32[0];
	float d2 = XMVector3Dot(p0_p2, p0_pt).m128_f32[0];

	if (d1 <= 0.0f&&d2 <= 0.0f)
	{
		//p0���ŋߒl
		*closest = triangle.p0;
		return;
	}

	//point��p1�̊O���̒��_�̈�̒��ɂ��邩�ǂ����`�F�b�N
	XMVECTOR p1_pt = point - triangle.p1;

	float d3 = XMVector3Dot(p0_p1, p1_pt).m128_f32[0];
	float d4 = XMVector3Dot(p0_p2, p1_pt).m128_f32[0];

	if (d3 > -0.0f&&d4 <= d3)
	{
		//p1���ŋߖT
		*closest = triangle.p1;
		return;
	}

	//point��p0_p1�̕ӗ̈�̒��ɂ��邩�ǂ����`�F�b�N���A�����point��p0_p1��ɑ΂���ˉe��Ԃ�
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0 && d1 >= 0.0f && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		*closest = triangle.p0 + v * p0_p1;
		return;
	}

	//point��p2�̊O���̒��_�̈�̒��ɂ��邩�ǂ����`�F�b�N
	XMVECTOR p2_pt = point - triangle.p2;

	float d5 = XMVector3Dot(p0_p1, p2_pt).m128_f32[0];
	float d6 = XMVector3Dot(p0_p2, p2_pt).m128_f32[0];
	if (d6 >= 0.0f && d5 <= d6)
	{
		*closest = triangle.p2;
		return;
	}

	// point��p0_p2�̕ӗ̈�̒��ɂ��邩�ǂ����`�F�b�N���A�����point��p0_p2��ɑ΂���ˉe��Ԃ�
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		*closest = triangle.p0 + w * p0_p2;
		return;
	}

	// point��p1_p2�̕ӗ̈�̒��ɂ��邩�ǂ����`�F�b�N���A�����point��p1_p2��ɑ΂���ˉe��Ԃ�
	float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
	{
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		*closest = triangle.p1 + w * (triangle.p2 - triangle.p1);
		return;
	}

	// point�͖ʗ̈�̒��ɂ���Bclosest���d�S���W��p���Čv�Z����
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	*closest = triangle.p0 + p0_p1 * v + p0_p2 * w;

}
bool Collision::CheckSphereBox(const Sphere& sphere, const Box& box, XMVECTOR* inter, XMVECTOR* reject)
{

	XMVECTOR rejectVec = XMVectorZero();
	float sqDistance = 0.0f;
	float d;

	d = sphere.center.m128_f32[0];
	if (d < box.minPosition.x)
	{
		sqDistance += (d - box.minPosition.x) * (d - box.minPosition.x);
		rejectVec.m128_f32[0] = box.minPosition.x - (d + sphere.radius);
	}
	if (d > box.maxPosition.x)
	{
		sqDistance += (d - box.maxPosition.x) * (d - box.maxPosition.x);
		rejectVec.m128_f32[0] =  box.maxPosition.x - (d - sphere.radius);
	}

	d = sphere.center.m128_f32[1];
	if (d < box.minPosition.y)
	{
		sqDistance += (d - box.minPosition.y) * (d - box.minPosition.y);
		rejectVec.m128_f32[1] = box.minPosition.y - (d + sphere.radius);
	}
	if (d > box.maxPosition.y)
	{
		sqDistance += (d - box.maxPosition.y) * (d - box.maxPosition.y);
		rejectVec.m128_f32[1] = box.maxPosition.y - (d - sphere.radius);

	}
	d = sphere.center.m128_f32[2];
	if (d < box.minPosition.z)
	{
		sqDistance += (d - box.minPosition.z) * (d - box.minPosition.z);
		rejectVec.m128_f32[2] = box.minPosition.z - (d + sphere.radius);
	}
	if (d > box.maxPosition.z)
	{
		sqDistance += (d - box.maxPosition.z) * (d - box.maxPosition.z);
		rejectVec.m128_f32[1] = box.maxPosition.z - (d - sphere.radius);
	}

	if (sqDistance > sphere.radius * sphere.radius)
		return false;


	if (reject)
	{
		*reject = rejectVec;
	}
	if(inter)
	{
		//�����Ă���̂�...?
		*inter = (sphere.center + rejectVec) +  XMVector3Normalize(-rejectVec) * sphere.radius;
	}
	return true;


}
bool Collision::CheckBoxBox(const Box& box1, const Box& box2, XMVECTOR* inter, XMVECTOR* reject)
{
	if (box2.maxPosition.x >= box1.minPosition.x && box2.minPosition.x <= box1.maxPosition.x &&
		box2.maxPosition.y >= box1.minPosition.y && box2.minPosition.y <= box1.maxPosition.y &&
		box2.maxPosition.z >= box1.minPosition.z && box2.minPosition.z <= box1.maxPosition.z)
	{
		if (inter)
		{
			Vector3 collPos = {};
			collPos.x = box2.maxPosition.x - ((box2.maxPosition.x - box1.minPosition.x) / 2);
			collPos.y = box2.maxPosition.y - ((box2.maxPosition.y - box1.minPosition.y) / 2);
			collPos.z = box2.maxPosition.z - ((box2.maxPosition.z - box1.minPosition.z) / 2);
			*inter = XMVectorSet(collPos.x, collPos.y, collPos.z, 1);
		}
		if(reject)
		{
			//�l����(�ړ��x�N�g���Ȃ��Ɩ�������Ȃ��H)
		}
		return true;
	}
	return false;
}

bool Collision::CheckSphere2Sphere(const Sphere & sphereA, const Sphere & sphereB, DirectX::XMVECTOR * inter, DirectX::XMVECTOR * reject)
{
	// ���S�_�̋����̂Q�� <= ���a�̘a�̂Q��@�Ȃ����
	float dist = XMVector3LengthSq(sphereA.center - sphereB.center).m128_f32[0];

	float radius2 = sphereA.radius + sphereB.radius;
	radius2 *= radius2;

	if (dist <= radius2) {
		if (inter) {
			// A�̔��a��0�̎����W��B�̒��S�@B�̔��a��0�̎����W��A�̒��S�@�ƂȂ�悤�⊮
			float t = sphereB.radius / (sphereA.radius + sphereB.radius);
			*inter = XMVectorLerp(sphereA.center, sphereB.center, t);
		}
		//�����o���x�N�g�����v�Z
		if (reject)
		{
			float rejectLen = sphereA.radius + sphereB.radius - sqrt(dist);
			*reject = XMVector3Normalize(sphereA.center - sphereB.center);
			*reject *= rejectLen;
		}
		return true;
	}

	return false;
}

bool Collision::CheckSphere2Plane(const Sphere & sphere, const Plane & plane, DirectX::XMVECTOR * inter)
{
	// ���W�n�̌��_���狅�̒��S���W�ւ̋�������
	// ���ʂ̌��_���������Z���邱�ƂŁA���ʂƋ��̒��S�Ƃ̋������o��
	float dist = XMVector3Dot(sphere.center, plane.normal).m128_f32[0] - plane.distance;
	// �����̐�Βl�����a���傫����Γ������Ă��Ȃ�
	if (fabsf(dist) > sphere.radius)	return false;

	// �[����_���v�Z
	if (inter)
	{
		// ���ʏ�̍Đڋߓ_���A�^����_�Ƃ���
		*inter = -dist * plane.normal + sphere.center;
	}
	return true;
}

bool Collision::CheckSphere2Triangle(const Sphere & sphere, const Triangle & triangle, DirectX::XMVECTOR * inter, DirectX::XMVECTOR * reject)
{
	XMVECTOR p;
	// ���̒��S�ɑ΂���ŋߐړ_�ł���O�p�`��ɂ���_p��������
	ClosestPtPoint2Triangle(sphere.center, triangle, &p);
	// �_p�Ƌ��̒��S�̍����x�N�g��
	XMVECTOR v = p - sphere.center;
	// �����̓������߂�
	//�i�����x�N�g�����m�̓��ς͎O�����̒藝�̃��[�g�����̎��ƈ�v����j
	float distanceSquare = XMVector3Dot(v, v).m128_f32[0];
	// ���ƎO�p�`�̋��������a�ȉ��Ȃ瓖�����Ă��Ȃ�
	if (distanceSquare > sphere.radius * sphere.radius)	return false;
	// �[����_���v�Z
	if (inter) {
		// �O�p�`��̍ŋߐړ_p���^����_�Ƃ���
		*inter = p;
	}
	//�����o���x�N�g�����v�Z
	if (reject)
	{
		float ds = XMVector3Dot(sphere.center, triangle.normal).m128_f32[0];
		float dt = XMVector3Dot(triangle.p0, triangle.normal).m128_f32[0];
		float rejectLen = dt - ds + sphere.radius;
		*reject = triangle.normal*rejectLen;
	}
	return true;
}

bool Collision::CheckRay2Plane(const Ray & lay, const Plane & plane, float * distance, DirectX::XMVECTOR * inter)
{
	const float epsilon = 1.0e-5f;	// �덷�z���p�̔����Ȓl

	float d1 = XMVector3Dot(plane.normal, lay.dir).m128_f32[0];
	// ���ʂɂ͓�����Ȃ�
	if (d1 > -epsilon) {
		return false;
	}

	float d2 = XMVector3Dot(plane.normal, lay.start).m128_f32[0];
	float t = (plane.distance - d2) / d1;

	if (t < 0) return false;

	// ��������������
	if (distance) {
		*distance = t;
	}

	// ��_���v�Z
	if (inter) {
		*inter = lay.start + t * lay.dir;
	}

	return true;
}

bool Collision::CheckRay2Triangle(const Ray & lay, const Triangle & triangle, float * distance, DirectX::XMVECTOR * inter)
{
	// �O�p�`������Ă��镽�ʂ��Z�o
	Plane plane;
	XMVECTOR interPlane;
	plane.normal = triangle.normal;
	plane.distance = XMVector3Dot(triangle.normal, triangle.p0).m128_f32[0];
	// ���C�ƕ��ʂ��������Ă��Ȃ���΁A�������Ă��Ȃ�	
	if (!CheckRay2Plane(lay, plane, distance, &interPlane)) {
		return false;
	}
	// ���C�ƕ��ʂ��������Ă����̂ŁA�����ƌ�_���������܂ꂽ

	// ���C�ƕ��ʂ̌�_���O�p�`�̓����ɂ��邩����
	const float epsilon = 1.0e-5f;	// �덷�z���p�̔����Ȓl
	XMVECTOR m;
	// ��p0_p1�ɂ���
	XMVECTOR pt_p0 = triangle.p0 - interPlane;
	XMVECTOR p0_p1 = triangle.p1 - triangle.p0;
	m = XMVector3Cross(pt_p0, p0_p1);
	// �ӂ̊O��
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) {
		return false;
	}

	// ��p1_p2�ɂ���
	XMVECTOR pt_p1 = triangle.p1 - interPlane;
	XMVECTOR p1_p2 = triangle.p2 - triangle.p1;
	m = XMVector3Cross(pt_p1, p1_p2);
	// �ӂ̊O��
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) {
		return false;
	}

	// ��p2_p0�ɂ���
	XMVECTOR pt_p2 = triangle.p2 - interPlane;
	XMVECTOR p2_p0 = triangle.p0 - triangle.p2;
	m = XMVector3Cross(pt_p2, p2_p0);
	// �ӂ̊O��
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) {
		return false;
	}

	if (inter) {
		*inter = interPlane;
	}

	// �����Ȃ̂ŁA�������Ă���
	return true;
}

bool Collision::CheckRay2Sphere(const Ray & lay, const Sphere & sphere, float * distance, DirectX::XMVECTOR * inter)
{
	XMVECTOR m = lay.start - sphere.center;
	float b = XMVector3Dot(m, lay.dir).m128_f32[0];
	float c = XMVector3Dot(m, m).m128_f32[0] - sphere.radius * sphere.radius;
	// lay�̎n�_��sphere�̊O���ɂ���(c > 0)�Alay��sphere���痣��Ă���������
	// �����Ă���ꍇ(b > 0)�A������Ȃ�
	if (c > 0.0f && b > 0.0f) {
		return false;
	}

	float discr = b * b - c;
	// ���̔��ʎ��̓��C�������O��Ă��邱�ƂɈ�v
	if (discr < 0.0f) {
		return false;
	}

	// ���C�͋��ƌ������Ă���B
	// ��������ŏ��̒lt���v�Z
	float t = -b - sqrtf(discr);
	// t�����ł���ꍇ�A���C�͋��̓�������J�n���Ă���̂�t���[���ɃN�����v
	if (t < 0) t = 0.0f;

	if (distance) {
		*distance = t;
	}

	if (inter) {
		*inter = lay.start + t * lay.dir;
	}

	return true;
}
