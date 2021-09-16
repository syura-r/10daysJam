#pragma once
#include "Scene.h"
#include"CreateFigure.h"
#include"Texture.h"
#include"Alpha.h"

#include "Object.h"
#include "FBXManager.h"
#include "DebugCamera.h"
#include "Sprite.h"

class Title :
	public Scene
{
public:
	Title();
	~Title()override;
	void Initialize() override;
	void Update()override;
	void PreDraw()override;
	void PostDraw()override;

	static void SetCamera(DebugCamera* cameraPtr) { camera = cameraPtr; }

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
	static DebugCamera* camera;

	int movienum;
	bool RiseObjects();//movienum1
	bool UnionObjects();//movienum2
	int unionNum;
	int unionAfterCounter;
	bool poseObjects();//movienum3

	const float AtoB_distance = 2.0f;//�G���������

	Sprite* logo = nullptr;//�^�C�g�����S
};
