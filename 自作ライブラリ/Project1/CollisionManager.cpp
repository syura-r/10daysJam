#include "CollisionManager.h"
#include"BaseCollider.h"
#include "BoxCollider.h"
#include"Collision.h"
#include"MeshCollider.h"
using namespace DirectX;
CollisionManager * CollisionManager::GetInstance()
{
	static CollisionManager instance;
	return &instance;
}

void CollisionManager::AddCollider(BaseCollider* collider)
{
	colliders.push_front(collider);
}

void CollisionManager::CheckAllCollisions()
{
	std::forward_list<BaseCollider*>::iterator itA;
	std::forward_list<BaseCollider*>::iterator itB;

	itA = colliders.begin();
	auto endItr = colliders.end();
	for (; itA != endItr; itA++)
	{
		itB = itA;
		++itB;
		for (; itB != endItr; ++itB)
		{
			BaseCollider* colA = *itA;
			BaseCollider* colB = *itB;
			//どちらも球の場合
			if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE &&
				colB->GetShapeType() == COLLISIONSHAPE_SPHERE)
			{
				Sphere* SphereA = dynamic_cast<Sphere*>(colA);
				Sphere* SphereB = dynamic_cast<Sphere*>(colB);
				DirectX::XMVECTOR inter;
				if (Collision::CheckSphere2Sphere(*SphereA, *SphereB, &inter))
				{
					colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
					colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
				}
			}
			//メッシュと球の場合
			else if (colA->GetShapeType() == COLLISIONSHAPE_MESH &&
				colB->GetShapeType() == COLLISIONSHAPE_SPHERE)
			{
				MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colA);
				Sphere* sphere = dynamic_cast<Sphere*>(colB);
				DirectX::XMVECTOR inter;
				if (meshCollider->CheckCollisionSphere(*sphere, &inter)) {
					colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
					colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
				}
			}
			else if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE &&
				colB->GetShapeType() == COLLISIONSHAPE_MESH)
			{
				MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colB);
				Sphere* sphere = dynamic_cast<Sphere*>(colA);
				DirectX::XMVECTOR inter;
				if (meshCollider->CheckCollisionSphere(*sphere, &inter))
				{
					colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
					colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
				}
			}
			//ボックス同士の場合(AABB)
			else if (colA->GetShapeType() == COLLISIONSHAPE_BOX &&
				colB->GetShapeType() == COLLISIONSHAPE_BOX)
			{
				BoxCollider* BoxA = dynamic_cast<BoxCollider*>(colA);
				BoxCollider* BoxB = dynamic_cast<BoxCollider*>(colB);
				DirectX::XMVECTOR inter;
				XMVECTOR tempReject;
				if (Collision::CheckBoxBox(*BoxA, *BoxB, &inter, &tempReject))
				{
					colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter, tempReject));
					colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter, tempReject));
				}
			}
			//ボックスと球の場合
			else if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE &&
				colB->GetShapeType() == COLLISIONSHAPE_BOX)
			{
				Sphere* sphere = dynamic_cast<Sphere*>(colA);
				BoxCollider* BoxB = dynamic_cast<BoxCollider*>(colB);
				DirectX::XMVECTOR inter;
				if (Collision::CheckSphereBox(*sphere, *BoxB, &inter))
				{
					colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
					colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
				}
			}
			//ボックスと球の場合
			else if (colB->GetShapeType() == COLLISIONSHAPE_SPHERE &&
				colA->GetShapeType() == COLLISIONSHAPE_BOX)
			{
				Sphere* sphere = dynamic_cast<Sphere*>(colB);
				BoxCollider* BoxB = dynamic_cast<BoxCollider*>(colA);
				DirectX::XMVECTOR inter;
				if (Collision::CheckSphereBox(*sphere, *BoxB, &inter))
				{
					colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
					colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
				}
			}

		}
	}

}

void CollisionManager::Initialize(float left, float top, float right, float bottom)
{
	//if (!L4Tree.Init(
	//	8,
	//	left, top, right, bottom))
	//	assert(0);

}

bool CollisionManager::Raycast(BaseCollider* collider,const Ray & ray, RaycastHit * hitinfo, float maxDistance)
{
	return Raycast(collider,ray, 0xffff, hitinfo, maxDistance);
}

bool CollisionManager::Raycast(BaseCollider* collider,const Ray & ray, unsigned short attribute, RaycastHit * hitinfo, float maxDistance)
{
	bool result = false;
	std::forward_list<BaseCollider*>::iterator it;
	//今までで最も近いコライダーを記録する為のイテレータ
	std::forward_list<BaseCollider*>::iterator it_hit;
	//今までで最も近いコライダーの距離を記録する変数
	float distance = maxDistance;
	//今までで最も近いコライダーとの交点を記録する変数
	XMVECTOR inter;
	BaseCollider* hitCollider;
	//全てのコライダーと総当たりチェック
	it = colliders.begin();
	auto endItr = colliders.end();
	for (; it != endItr; ++it)
	{
		BaseCollider* colA = *it;

		if (!(colA->attribute & attribute))
			continue;
		
		//球の場合
		if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE)
		{
			Sphere* sphere = dynamic_cast<Sphere*>(colA);
			float tempDistance;
			XMVECTOR tempinter;
			//当たらなければ除外
			if (!Collision::CheckRay2Sphere(ray, *sphere, &tempDistance, &tempinter))continue;
			//距離が最小でなければ除外
			if (tempDistance >= distance)continue;
			//今までで最も近いのであれば記録を取る
			result = true;
			distance = tempDistance;
			inter = tempinter;
			hitCollider = colA;
		}
		//メッシュの場合
		else if (colA->GetShapeType() == COLLISIONSHAPE_MESH)
		{
			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colA);

			float tempDistance;
			DirectX::XMVECTOR templnter;
			if (!meshCollider->CheckCollisionRay(ray, &tempDistance, &templnter))continue;
			if (tempDistance >= distance)continue;

			result = true;
			distance = tempDistance;
			inter = templnter;
			hitCollider = colA;
		}
		//球の場合
		else if (colA->GetShapeType() == COLLISIONSHAPE_BOX)
		{
			Box* box = dynamic_cast<Box*>(colA);
			float tempDistance;
			XMVECTOR tempinter;
			//当たらなければ除外
			if (!Collision::CheckRay2Box(ray, *box, &tempDistance, &tempinter))continue;
			//距離が最小でなければ除外
			if (tempDistance >= distance)continue;
			//今までで最も近いのであれば記録を取る
			result = true;
			distance = tempDistance;
			inter = tempinter;
			hitCollider = colA;
		}

	}
	//最終的に何かに当たっていたら結果を書き込む
	if (result && hitinfo)
	{
		hitinfo->distance = distance;
		hitinfo->inter = inter;
		hitinfo->collider = hitCollider;
		hitinfo->object = hitinfo->collider->GetObject3D();
	}
	return result;
}

void CollisionManager::QuerySphere(const Sphere & sphere, QueryCallback * callback, unsigned short attribute1, unsigned short attribute2,BaseCollider* collider)
{
	assert(callback);

	std::forward_list<BaseCollider*>::iterator it;
	//今までで最も近いコライダーを記録する為のイテレータ
	std::forward_list<BaseCollider*>::iterator it_hit;
	//今までで最も近いコライダーの距離を記録する変数
	//全てのコライダーと総当たりチェック
	it = colliders.begin();
	auto endItr = colliders.end();
	for (; it != endItr; ++it)
	{
		BaseCollider* col = *it;
		if (!(col->attribute & attribute1) && !(col->attribute & attribute2))
			continue;
		//BaseCollider* col = *it;
		
		//球
		if (col->GetShapeType() == COLLISIONSHAPE_SPHERE)
		{
			Sphere* sphereB = dynamic_cast<Sphere*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			if (!Collision::CheckSphere2Sphere(sphere, *sphereB, &tempInter, &tempReject))
				continue;

			//交差情報をセット
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3D();
			info.inter = tempInter;
			info.reject = tempReject;

			//クエリーコールバック呼び出し
			if (!callback->OnQueryHit(info))
			{
				//戻り値がfalseの場合、継続せずに終了
				return;
			}
		}
		//メッシュ
		if (col->GetShapeType() == COLLISIONSHAPE_MESH)
		{
			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			if (!meshCollider->CheckCollisionSphere(sphere,&tempInter,&tempReject))
				continue;

			//交差情報をセット
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3D();
			info.inter = tempInter;
			info.reject = tempReject;

			//クエリーコールバック呼び出し
			if (!callback->OnQueryHit(info))
			{
				//戻り値がfalseの場合、継続せずに終了
				return;
			}
		}
		//ボックス
		if (col->GetShapeType() == COLLISIONSHAPE_BOX)
		{
			Box* boxB = dynamic_cast<Box*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			if (!Collision::CheckSphereBox(sphere, *boxB, &tempInter, &tempReject))
				continue;
			if (collider != nullptr)
				col->OnCollision(CollisionInfo(collider->GetObject3D(), collider, tempInter, tempReject));
			//交差情報をセット
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3D();
			info.inter = tempInter;
			info.reject = tempReject;

			//クエリーコールバック呼び出し
			if (!callback->OnQueryHit(info))
			{
				//戻り値がfalseの場合、継続せずに終了
				return;
			}
		}

	}

}

void CollisionManager::QueryBox(const Box& box, QueryCallback* callback, unsigned short attribute1, unsigned short attribute2,BaseCollider* collider)
{
	assert(callback);
	std::forward_list<BaseCollider*>::iterator it;
	//今までで最も近いコライダーを記録する為のイテレータ
	std::forward_list<BaseCollider*>::iterator it_hit;
	//今までで最も近いコライダーの距離を記録する変数
	//全てのコライダーと総当たりチェック
	it = colliders.begin();
	auto endItr = colliders.end();
	for (; it != endItr; ++it)
	{
		BaseCollider* col = *it;
		if (col->attribute != attribute1 && col->attribute != attribute2)
			continue;
		
		//球
		if (col->GetShapeType() == COLLISIONSHAPE_SPHERE)
		{
			Sphere* sphereB = dynamic_cast<Sphere*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			if (!Collision::CheckSphereBox( *sphereB,box, &tempInter, &tempReject))
				continue;

			//交差情報をセット
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3D();
			info.inter = tempInter;
			info.reject = tempReject;

			//クエリーコールバック呼び出し
			if (!callback->OnQueryHit(info))
			{
				//戻り値がfalseの場合、継続せずに終了
				return;
			}
		}
		//ボックス
		if (col->GetShapeType() == COLLISIONSHAPE_BOX)
		{
			Box* boxB = dynamic_cast<Box*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			if (!Collision::CheckBoxBox(box,*boxB, &tempInter, &tempReject))
				continue;
			if(collider != nullptr)
				col->OnCollision(CollisionInfo(collider->GetObject3D(), collider, tempInter, tempReject));
			//交差情報をセット
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3D();
			info.inter = tempInter;
			info.reject = tempReject;
			//if (tempReject.m128_f32[1] < 0)
			//{
			//	static  int a;
			//	a++;
			//}

			//クエリーコールバック呼び出し
			if (!callback->OnQueryHit(info))
			{
				//戻り値がfalseの場合、継続せずに終了
				return;
			}
		}
	}

}
