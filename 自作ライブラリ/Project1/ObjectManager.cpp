#include "ObjectManager.h"
#include "Object.h"
#include "PtrDelete.h"

ObjectManager* ObjectManager::GetInstance()
{
	static ObjectManager instance;
    return &instance;
}

void ObjectManager::Add(Object* object, bool preDraw)
{
	objects[preDraw].push_back(object);
}

void ObjectManager::Initialize()
{
	auto end_it = objects.end();
	for (auto it = objects.begin(); it != end_it; ++it)
	{
		auto end_itr = it->second.end();
		for (auto itr = it->second.begin(); itr != end_itr; ++itr)
		{
			(*itr)->Initialize();
		}
	}
}

void ObjectManager::Update()
{
	auto end_it = objects.end();
	for(auto it = objects.begin();it != end_it;++it)
	{
		auto end_itr = it->second.end();
		for(auto itr = it->second.begin();itr != end_itr;++itr)
		{
			(*itr)->Update();
		}
	}
}

void ObjectManager::PreDraw()
{
	auto end_itr = objects[true].end();
	for (auto itr =  objects[true].begin(); itr != end_itr; ++itr)
	{
		(*itr)->Draw();
	}
}

void ObjectManager::PostDraw()
{
	auto end_itr = objects[false].end();
	for (auto itr = objects[false].begin(); itr != end_itr; ++itr)
	{
		(*itr)->Draw();
	}

}

void ObjectManager::End()
{
	auto end_it = objects.end();
	for (auto it = objects.begin(); it != end_it; ++it)
	{
		auto end_itr = it->second.end();
		for (auto itr = it->second.begin(); itr != end_itr; ++itr)
		{
			PtrDelete(*itr);
		}
	}
	objects.clear();
}
