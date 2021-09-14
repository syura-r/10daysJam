#include "SceneManager.h"


SceneManager* SceneManager::GetInstance()
{
	static SceneManager instance;
	return &instance;
}

void SceneManager::Add(Scene::SCENE name, Scene * scene)
{
	if (scenes[name] != nullptr)
	{
		return;
	}
	scenes[name] = scene;
}

void SceneManager::Update()
{
	if (currentScene == nullptr)
	{
		return;
	}
	currentScene->Update();
	if (currentScene->GetIsEnd())
	{
		Change(currentScene->NextScene());
	}
}

void SceneManager::Change(Scene::SCENE name)
{
	currentScene = scenes[name];
	currentScene->Initialize();
}

void SceneManager::PreDraw()
{
	if (currentScene == nullptr)
	{
		return;
	}
	currentScene->PreDraw();
}

void SceneManager::PostDraw()
{
	if (currentScene == nullptr)
	{
		return;
	}
	currentScene->PostDraw();

}

void SceneManager::End()
{
	for (auto itr = scenes.begin(); itr != scenes.end(); itr++)
	{
		delete (*itr).second;
		(*itr).second = nullptr;
	}
}
