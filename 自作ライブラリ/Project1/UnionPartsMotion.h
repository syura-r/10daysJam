#pragma once
#include "Object.h"
#include "FBXManager.h"
#include "DebugCamera.h"

class UnionPartsMotion
{
public:
	UnionPartsMotion();
	UnionPartsMotion(DebugCamera* camera);
	~UnionPartsMotion();

	void Initialize();
	void Update();
	void Draw();


private:
	struct Parts
	{
		Object* obj = nullptr;
		Vector3 startPosition;
		Vector3 startRotation;
		float easingCounter;
		Parts(std::string objKeyname)
		{
			obj = new Object();
			obj->Create(FBXManager::GetModel(objKeyname));
			obj->SetColor({ 0, 0, 0, 1 });
		}
		~Parts()
		{
			delete obj;
		}

		void Initialize(const Vector3& position, const Vector3& rotation)
		{
			this->startPosition = position;
			this->startRotation = rotation;
			obj->SetPosition(position);
			obj->SetRotation(rotation);
			easingCounter = 0.0f;
		}
	};
	std::vector<Parts*> parts_array;

	//ƒ‚[ƒVƒ‡ƒ“‚Ì’iŠK
	int movienum;

	bool RiseObjects();//movienum1
	bool UnionObjects();//movienum2
	int unionNum;
	int unionAfterCounter;
	bool poseObjects();//movienum3

	DebugCamera* camera = nullptr;
};

