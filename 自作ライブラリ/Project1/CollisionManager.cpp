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
	colliders.push_back(collider);
	Tree::TreeObject<BaseCollider>* p = new Tree::TreeObject<BaseCollider>;
	p->pObject = collider;
	SmartPtr<Tree::TreeObject<BaseCollider>> s;
	s.SetPtr(p);
	const Vector3 min = collider->GetMin();
	const Vector3 max = collider->GetMax();
	spSOFTAry.push_back(s);
	L8Tree.Regist(min, max, s);
}

void CollisionManager::CheckAllCollisions()
{
	auto end_it = spSOFTAry.end();
	for(auto it = spSOFTAry.begin();it != end_it;++it)
	{
		if((*it)->pObject->move == true)
		{
			(*it)->Remove();
			const Vector3 min = (*it)->pObject->GetMin();
			const Vector3 max = (*it)->pObject->GetMax();
			L8Tree.Regist(min, max, *it);
		}
	}
	
	DWORD collisionCount = L8Tree.GetAllCollisionList(ColVect);
	DWORD c;
	auto colCount = collisionCount / 2;	// 2�Ŋ���̂̓y�A�ɂȂ��Ă���̂�
	for (c = 0; c < colCount; c++) {
		BaseCollider* colA = ColVect[c * 2];
		BaseCollider* colB = ColVect[c * 2 + 1];
		if (colA->attribute == colB->attribute)
			continue;

		//�ǂ�������̏ꍇ
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
		//���b�V���Ƌ��̏ꍇ
		else if (colA->GetShapeType() == COLLISIONSHAPE_MESH &&
			colB->GetShapeType() == COLLISIONSHAPE_SPHERE)
		{
			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colA);
			Sphere* sphere = dynamic_cast<Sphere*>(colB);
			DirectX::XMVECTOR inter;
			XMVECTOR tempReject;
			if (meshCollider->CheckCollisionSphere(*sphere, &inter,&tempReject)) {
				colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
				colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter, tempReject));
			}
		}
		//���b�V���Ƌ��̏ꍇ
		else if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE &&
			colB->GetShapeType() == COLLISIONSHAPE_MESH)
		{
			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colB);
			Sphere* sphere = dynamic_cast<Sphere*>(colA);
			DirectX::XMVECTOR inter;
			XMVECTOR tempReject;
			if (meshCollider->CheckCollisionSphere(*sphere, &inter,&tempReject)) 
			{
				colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter, tempReject));
				colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
			}
		}
		//�{�b�N�X���m�̏ꍇ(AABB)
		else if (colA->GetShapeType() == COLLISIONSHAPE_BOX &&
			colB->GetShapeType() == COLLISIONSHAPE_BOX)
		{
			BoxCollider* BoxA = dynamic_cast<BoxCollider*>(colA);
			BoxCollider* BoxB = dynamic_cast<BoxCollider*>(colB);
			DirectX::XMVECTOR inter;
			if (Collision::CheckBoxBox(*BoxA, *BoxB, &inter))
			{
				colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
				colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
			}
		}
		//�{�b�N�X�Ƌ��̏ꍇ
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
		//�{�b�N�X�Ƌ��̏ꍇ
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

	//std::vector<BaseCollider*>::iterator itA;
	//std::vector<BaseCollider*>::iterator itB;

	//itA = colliders.begin();
	//for (; itA != colliders.end(); ++itA)
	//{
	//	itB = itA;
	//	++itB;
	//	for (; itB != colliders.end(); ++itB)
	//	{
	//		BaseCollider* colA = *itA;
	//		BaseCollider* colB = *itB;

	//		//�ǂ�������̏ꍇ
	//		if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE &&
	//			colB->GetShapeType() == COLLISIONSHAPE_SPHERE)
	//		{
	//			Sphere* SphereA = dynamic_cast<Sphere*>(colA);
	//			Sphere* SphereB = dynamic_cast<Sphere*>(colB);
	//			DirectX::XMVECTOR inter;
	//			if (Collision::CheckSphere2Sphere(*SphereA, *SphereB, &inter))
	//			{
	//				colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
	//				colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
	//			}
	//		}
	//		//���b�V���Ƌ��̏ꍇ
	//		else if (colA->GetShapeType() == COLLISIONSHAPE_MESH &&
	//			colB->GetShapeType() == COLLISIONSHAPE_SPHERE) 
	//		{
	//			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colA);
	//			Sphere* sphere = dynamic_cast<Sphere*>(colB);
	//			DirectX::XMVECTOR inter;
	//			if (meshCollider->CheckCollisionSphere(*sphere, &inter)) {
	//				colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
	//				colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
	//			}
	//		}
	//		//���b�V���Ƌ��̏ꍇ
	//		else if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE &&
	//			colB->GetShapeType() == COLLISIONSHAPE_MESH)
	//		{
	//			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colB);
	//			Sphere* sphere = dynamic_cast<Sphere*>(colA);
	//			DirectX::XMVECTOR inter;
	//			if (meshCollider->CheckCollisionSphere(*sphere, &inter))
	//			{
	//				colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
	//				colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
	//			}
	//		}
	//		//�{�b�N�X���m�̏ꍇ(AABB)
	//		else if (colA->GetShapeType() == COLLISIONSHAPE_BOX &&
	//			colB->GetShapeType() == COLLISIONSHAPE_BOX)
	//		{
	//			BoxCollider* BoxA = dynamic_cast<BoxCollider*>(colA);
	//			BoxCollider* BoxB = dynamic_cast<BoxCollider*>(colB);
	//			DirectX::XMVECTOR inter;
	//			if( Collision::CheckBoxBox(*BoxA, *BoxB, &inter))
	//			{
	//				colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
	//				colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
	//			}
	//		}
	//		//�{�b�N�X�Ƌ��̏ꍇ
	//		else if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE &&
	//			colB->GetShapeType() == COLLISIONSHAPE_BOX)
	//		{
	//			Sphere* sphere = dynamic_cast<Sphere*>(colA);
	//			BoxCollider* BoxB = dynamic_cast<BoxCollider*>(colB);
	//			DirectX::XMVECTOR inter;
	//			if (Collision::CheckSphereBox(*sphere, *BoxB, &inter))
	//			{
	//				colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
	//				colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
	//			}
	//		}
	//		//�{�b�N�X�Ƌ��̏ꍇ
	//		else if (colB->GetShapeType() == COLLISIONSHAPE_SPHERE &&
	//			colA->GetShapeType() == COLLISIONSHAPE_BOX)
	//		{
	//			Sphere* sphere = dynamic_cast<Sphere*>(colB);
	//			BoxCollider* BoxB = dynamic_cast<BoxCollider*>(colA);
	//			DirectX::XMVECTOR inter;
	//			if (Collision::CheckSphereBox(*sphere, *BoxB, &inter))
	//			{
	//				colA->OnCollision(CollisionInfo(colB->GetObject3D(), colB, inter));
	//				colB->OnCollision(CollisionInfo(colA->GetObject3D(), colA, inter));
	//			}
	//		}
	//	}
	//}
}

void CollisionManager::Initialize(const Vector3& minLine,const Vector3& maxLine)
{
	if (!L8Tree.Initialize(
		6,
		minLine,
		maxLine))
		assert(0);

}

bool CollisionManager::Raycast(const Ray & ray, RaycastHit * hitinfo, float maxDistance)
{
	return Raycast(ray, 0xffff, hitinfo, maxDistance);
}

bool CollisionManager::Raycast(const Ray & ray, unsigned short attribute, RaycastHit * hitinfo, float maxDistance)
{
	bool result = false;
	//�����p�̃C�e���[�^
	std::vector<BaseCollider*>::iterator it;
	//���܂łōł��߂��R���C�_�[���L�^����ׂ̃C�e���[�^
	std::vector<BaseCollider*>::iterator it_hit;
	//���܂łōł��߂��R���C�_�[�̋������L�^����ϐ�
	float distance = maxDistance;
	//���܂łōł��߂��R���C�_�[�Ƃ̌�_���L�^����ϐ�
	XMVECTOR inter;

	//�S�ẴR���C�_�[�Ƒ�������`�F�b�N
	it = colliders.begin();
	for (; it != colliders.end(); ++it)
	{
		BaseCollider* colA = *it;

		//����������Ȃ���΃X�L�b�v
		if (!(colA->attribute & attribute))
			continue;

		//���̏ꍇ
		if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE)
		{
			Sphere* sphere = dynamic_cast<Sphere*>(colA);
			float tempDistance;
			XMVECTOR tempinter;
			//������Ȃ���Ώ��O
			if (!Collision::CheckRay2Sphere(ray, *sphere, &tempDistance, &tempinter))continue;
			//�������ŏ��łȂ���Ώ��O
			if (tempDistance >= distance)continue;
			//���܂łōł��߂��̂ł���΋L�^�����
			result = true;
			distance = tempDistance;
			inter = tempinter;
			it_hit = it;
		}
		//���b�V���̏ꍇ
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
			it_hit = it;
		}
	}
	//�ŏI�I�ɉ����ɓ������Ă����猋�ʂ���������
	if (result && hitinfo)
	{
		hitinfo->distance = distance;
		hitinfo->inter = inter;
		hitinfo->collider = *it_hit;
		hitinfo->object = hitinfo->collider->GetObject3D();
	}
	return result;
}

void CollisionManager::QuerySphere(const Sphere & sphere, QueryCallback * callback, unsigned short attribute)
{
	assert(callback);

	std::vector<BaseCollider*>::iterator it;

	//�S�ẴR���C�_�[�Ƃ̑�������`�F�b�N
	it = colliders.begin();
	for (; it != colliders.end(); ++it)
	{
		BaseCollider* col = *it;

		//����������Ȃ���΃X�L�b�v
		if (!(col->attribute & attribute))
			continue;
		
		//��
		if (col->GetShapeType() == COLLISIONSHAPE_SPHERE)
		{
			Sphere* sphereB = dynamic_cast<Sphere*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			if (!Collision::CheckSphere2Sphere(sphere, *sphereB, &tempInter, &tempReject))
				continue;

			//���������Z�b�g
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3D();
			info.inter = tempInter;
			info.reject = tempReject;

			//�N�G���[�R�[���o�b�N�Ăяo��
			if (!callback->OnQueryHit(info))
			{
				//�߂�l��false�̏ꍇ�A�p�������ɏI��
				return;
			}
		}
		//���b�V��
		if (col->GetShapeType() == COLLISIONSHAPE_MESH)
		{
			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(col);

			XMVECTOR tempInter;
			XMVECTOR tempReject;
			if (!meshCollider->CheckCollisionSphere(sphere,&tempInter,&tempReject))
				continue;

			//���������Z�b�g
			QueryHit info;
			info.collider = col;
			info.object = col->GetObject3D();
			info.inter = tempInter;
			info.reject = tempReject;

			//�N�G���[�R�[���o�b�N�Ăяo��
			if (!callback->OnQueryHit(info))
			{
				//�߂�l��false�̏ꍇ�A�p�������ɏI��
				return;
			}
		}

	}

}
