#include"Object.h"
#include"PtrDelete.h"
#include"BaseCollider.h"
#include"CollisionManager.h"
Object::~Object()
{
	if (collider)
	{
		//コリジョンマネージャーから登録を解除する
		CollisionManager::GetInstance()->RemoveCollider(collider);
		PtrDelete(collider);
	}
	PtrDelete(object);
}

void Object::Create(Model * model)
{
	object = Object3D::Create(model, position, scale, rotation, color);
}

void Object::Initialize()
{
	name = typeid(*this).name();	
}

void Object::Update()
{
	object->Update();
	if (collider)
		collider->Update();
}

void Object::Draw()
{
	if (object)
		object->Draw();
}

void Object::CustomDraw(const bool fbx, const bool shade, BLENDTYPE type,const bool customPipeline)
{
	if (object)
		object->Draw(fbx, shade, type, customPipeline);
}

void Object::SetCollider(BaseCollider * collider)
{
	collider->SetObject(this);
	
	this->collider = collider;
	object->Update();
	collider->Update();
	CollisionManager::GetInstance()->AddCollider(collider);
}
