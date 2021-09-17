#pragma once
#include"CollisionPrimitive.h"
#include"window.h"
class Collision
{
private:
	static Window* window;
public:
	static void SetWindow(Window* window) { Collision::window = window; }
	
	// �_�ƎO�p�`�̍ŋߐړ_�����߂�
	static void ClosestPtPoint2Triangle(const DirectX::XMVECTOR& point, const Triangle& triangle, DirectX::XMVECTOR* closest);
	
	// ���Ƌ��̓����蔻��
	static bool CheckSphere2Sphere(const Sphere& sphereA, const Sphere& sphereB, DirectX::XMVECTOR*inter = nullptr,DirectX::XMVECTOR * reject = nullptr);
	
	// ���ƕ��ʂ̓����蔻��
	static bool CheckSphere2Plane(const Sphere& sphere, const Plane& plane, DirectX::XMVECTOR*inter = nullptr);
	
	// ���Ɩ@���t���O�p�`�̓����蔻��
	static bool CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle, DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR * reject = nullptr);
	
	//���C�ƕ��ʂ̓����蔻��
	static bool CheckRay2Plane(const Ray& lay, const Plane& plane, float* distance, DirectX::XMVECTOR* inter = nullptr);

	// ���C�Ɩ@���t���O�p�`�̓����蔻��
	static bool CheckRay2Triangle(const Ray& lay, const Triangle& triangle, float*distance = nullptr, DirectX::XMVECTOR* inter = nullptr);
	
	// ���C�Ƌ��̓����蔻��
	static bool CheckRay2Sphere(const Ray& lay, const Sphere& sphere, float*distance = nullptr, DirectX::XMVECTOR* inter = nullptr);

	//���C�ƃ{�b�N�X�̓����蔻��
	static bool CheckRay2Box(const Ray& lay, const Box& box, float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);
	
	//���Ƌ�`�̓����蔻��
	static bool CheckSphereBox(const Sphere& sphere, const Box& box, DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);

	//��`�Ƌ�`�̓����蔻��
	static bool CheckBoxBox(const Box& box1, const Box& box2, DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);

};

