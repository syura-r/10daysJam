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
		colliders.remove(collider);
	}

	void CheckAllCollisions();
	/// <summary>
	/// 4分木の領域を決定する
	/// </summary>
	/// <param name="left"></param>領域の左端
	/// <param name="top"></param>領域の上端
	/// <param name="right"></param>領域の右端
	/// <param name="bottom"></param>領域の下端
	void Initialize(float left, float top, float right, float bottom);
	bool Raycast(BaseCollider* collider,const Ray& ray, RaycastHit* hitinfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);
	bool Raycast(BaseCollider* collider,const Ray& ray,unsigned short attribute, RaycastHit* hitinfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);
	//球による衝突全検索
	void QuerySphere(const Sphere& sphere, QueryCallback* callback, unsigned short attribute1 = (unsigned short)0xffffffff, unsigned short attribute2 = (unsigned short)0xffffffff, BaseCollider* collider = nullptr);
	//ボックスによる衝突全検索
	void QueryBox(const Box& box, QueryCallback* callback, unsigned short attribute1 = (unsigned short)0xffffffff, unsigned short attribute2 = (unsigned short)0xffffffff,BaseCollider * collider = nullptr);

private:
	CollisionManager() = default;
	CollisionManager(const CollisionManager&) = default;
	~CollisionManager() = default;
	CollisionManager& operator=(const CollisionManager&) = default;
	std::forward_list<BaseCollider*> colliders;



};

