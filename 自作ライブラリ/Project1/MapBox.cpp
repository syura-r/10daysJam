#include "MapBox.h"

#include "BoxCollider.h"
#include "CollisionAttribute.h"

MapBox::MapBox(OBJModel* model, const Vector3& position, const Vector3& scale, const Vector3& rotation)
{
	this->position = position;
	this->scale = scale;
	this->rotation = rotation;

	name = typeid(*this).name();

	Create(model);
	//コライダーの追加
	BoxCollider* collider = new BoxCollider;
	collider->SetAttribute(COLLISION_ATTR_LANDSHAPE);
	Vector3 colliderScale = 0.5f * scale;
	collider->SetScale(colliderScale);
	//collider->SetMove(true);
	SetCollider(collider);
}
