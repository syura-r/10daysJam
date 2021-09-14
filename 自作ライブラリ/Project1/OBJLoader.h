#pragma once
#include<unordered_map>
#include<string>
#include"OBJModel.h"
class OBJLoader
{
public:
	static void LoadModelFile(const std::string& keyName, const std::string& fileName, const bool& smooth);
	static OBJModel* GetModel(const std::string& keyName) {
		if (models[keyName] == nullptr)
			assert(0);
		return models[keyName];
	};
	static void DeleteModels();
private:
	static std::unordered_map<std::string, OBJModel*>models;
};
