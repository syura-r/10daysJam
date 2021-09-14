#pragma once
#include <unordered_map>

class Object;
class ObjectManager
{
public:
	static ObjectManager* GetInstance();
	void Add( Object* object = nullptr,bool preDraw = true);
	void Initialize();
	void Update();
	void PreDraw();
	void PostDraw();
	void End();
private:
	ObjectManager() = default;
	ObjectManager(const ObjectManager&) = delete;
	~ObjectManager() = default;
	ObjectManager& operator=(const ObjectManager&) = delete;

	std::unordered_map<bool,std::vector<Object*>>objects;
};

