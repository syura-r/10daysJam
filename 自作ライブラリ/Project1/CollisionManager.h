#pragma once
#include<forward_list>
#include"CollisionPrimitive.h"
#include"RaycastHit.h"
#include"QueryCallback.h"
#include<d3d12.h>
#include"Tree.h"

class BaseCollider;
class CollisionManager
{
public://�ÓI�����o�֐�
	static CollisionManager* GetInstance();

public://�����o�֐�
	void AddCollider(BaseCollider* collider);

	inline void RemoveCollider(BaseCollider* collider)
	{
		for(auto it = colliders.begin();it != colliders.end();)
		{
			if (*it == collider)
			{
				it = colliders.erase(it);
			}
			else
			{
				++it;
			}
		}
		//colliders.remove(collider);
	}

	void CheckAllCollisions();
	/// <summary>
	/// 4���؂̗̈�����肷��
	/// </summary>
	/// <param name="left"></param>�̈�̍��[
	/// <param name="top"></param>�̈�̏�[
	/// <param name="right"></param>�̈�̉E�[
	/// <param name="bottom"></param>�̈�̉��[
	void Initialize(float left, float top, float right, float bottom);
	bool Raycast(BaseCollider* collider,const Ray& ray, RaycastHit* hitinfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);
	bool Raycast(BaseCollider* collider,const Ray& ray,unsigned short attribute, RaycastHit* hitinfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);
	//���ɂ��ՓˑS����
	void QuerySphere(const Sphere& sphere, QueryCallback* callback, unsigned short attribute = (unsigned short)0xffffffff);
	//�{�b�N�X�ɂ��ՓˑS����
	void QueryBox(const Box& box, QueryCallback* callback, unsigned short attribute = (unsigned short)0xffffffff);

private:
	CollisionManager() = default;
	CollisionManager(const CollisionManager&) = default;
	~CollisionManager() = default;
	CollisionManager& operator=(const CollisionManager&) = default;
	std::vector<BaseCollider*> colliders;
	Tree::CLiner4TreeManager<BaseCollider> L4Tree;
	std::vector<BaseCollider*> ColVect;	// �ՓˑΏۃ��X�g
	std::vector < SmartPtr<Tree::TreeObject<BaseCollider>>> spSOFTAry;



};

