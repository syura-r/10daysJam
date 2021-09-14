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
public://静的メンバ関数
	static CollisionManager* GetInstance();

public://メンバ関数
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
	/// 8分木の領域を決定する
	/// </summary>
	/// <param name="minLine"></param>領域の最低位置
	/// <param name="maxLine"></param>領域の最高位置
	void Initialize(const Vector3& minLine, const Vector3& maxLine);
	bool Raycast(const Ray& ray, RaycastHit* hitinfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);
	bool Raycast(const Ray& ray,unsigned short attribute, RaycastHit* hitinfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);
	//球による衝突全検索
	void QuerySphere(const Sphere& sphere, QueryCallback* callback, unsigned short attribute = (unsigned short)0xffffffff);
private:
	CollisionManager() = default;
	CollisionManager(const CollisionManager&) = default;
	~CollisionManager() = default;
	CollisionManager& operator=(const CollisionManager&) = default;
	std::vector<BaseCollider*> colliders;
	Tree::CLiner8TreeManager<BaseCollider> L8Tree;
	std::vector<BaseCollider*> ColVect;	// 衝突対象リスト
	std::vector < SmartPtr<Tree::TreeObject<BaseCollider>>> spSOFTAry;



};

