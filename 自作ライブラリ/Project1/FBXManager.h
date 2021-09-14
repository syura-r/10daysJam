#pragma once
#pragma once
#include<unordered_map>
#include<string>
#include"FBXModel.h"
class FBXManager
{
public:
	static void LoadModelFile(const std::string& keyName, const std::string& fileName, const bool& smooth);
	static FBXModel* GetModel(const std::string& keyName) {
		if (models[keyName] == nullptr)
			assert(0);
		return models[keyName];
	};
	static void DeleteModels();
private:
	static std::unordered_map<std::string, FBXModel*>models;

};

