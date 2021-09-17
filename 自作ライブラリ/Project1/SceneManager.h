#pragma once
#include"Scene.h"
#include<unordered_map>
class SceneManager
{
public:
	static SceneManager* GetInstance();
	void Add(Scene::SCENE name,Scene* scene);
	void Update();
	void Change(Scene::SCENE next);
	void PreDraw();
	void PostDraw();
	void End();

	bool GetIsAllEnd() { return currentScene->GetIsAllEnd(); }

private:
	SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	~SceneManager() = default;
	SceneManager& operator=(const SceneManager&) = delete;
	
	std::unordered_map<Scene::SCENE, Scene*>scenes;
	Scene* currentScene = nullptr;
};

