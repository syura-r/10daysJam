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
	//–³—‚â‚èŽg‚¢•û•Ï‚¦‚é
	
	auto end_itr = objects[true].end();
	for (auto itr = objects[true].begin(); itr != end_itr; ++itr)
	{
		(*itr)->Initialize();
	}
	auto end_itr2 = objects[false].end();
	for (auto itr = objects[false].begin(); itr != end_itr2; ++itr)
	{
		auto deletePtr = *itr;
		itr =  objects[false].erase(itr);
		PtrDelete(deletePtr);
	}
}

void ObjectManager::Update()
{
	auto end_it = objects.end();
	for(auto it = objects.begin();it != end_it;++it)
	{
		for(auto itr = it->second.begin();itr != it->second.end();)
		{
			(*itr)->Update();
			if ((*itr)->IsDead())
			{
				auto deletePtr = *itr;
				itr = it->second.erase(itr);
				PtrDelete(deletePtr);
			}
			else
				++itr;
		}
	}
}

void ObjectManager::PreDraw()
{
	auto end_it = objects.end();
	for (auto it = objects.begin(); it != end_it; ++it)
	{
		auto end_itr = it->second.end();
		for (auto itr = it->second.begin(); itr != end_itr; ++itr)
		{
			(*itr)->Draw();
		}
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
