#include "OBJLoader.h"

std::unordered_map<std::string, OBJModel*> OBJLoader::models = {};

void OBJLoader::LoadModelFile(const std::string & keyName, const std::string & fileName, const bool& smooth)
{
	OBJModel* model = OBJModel::CreateOBJ(fileName, smooth);

	models[keyName] = model;
}

void OBJLoader::DeleteModels()
{
	for (auto itr = models.begin(); itr != models.end(); itr++)
	{
		delete (*itr).second;
		(*itr).second = nullptr;
	}
}
