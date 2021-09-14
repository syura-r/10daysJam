#include "FBXManager.h"
#include"FbxLoader.h"
std::unordered_map<std::string, FBXModel*> FBXManager::models = {};

void FBXManager::LoadModelFile(const std::string& keyName, const std::string& fileName, const bool& smooth)
{
	FBXModel* model = FbxLoader::GetInstance()->LoadModelFromFile(fileName);

	models[keyName] = model;
}

void FBXManager::DeleteModels()
{
	for (auto itr = models.begin(); itr != models.end(); itr++)
	{
		delete (*itr).second;
		(*itr).second = nullptr;
	}
}
