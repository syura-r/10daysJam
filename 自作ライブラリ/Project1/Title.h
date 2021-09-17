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
	static void SetWindowsize(Vector2 windowsize) { Title::windowsize = windowsize; }

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

	//�^�C�g�����S
	Sprite* logo = nullptr;

	//�I��ԍ�
	int selectNum;
	//�I������1
	Sprite* start = nullptr;
	XMFLOAT2 startScale;
	XMFLOAT4 startColor;
	//�I������2
	Sprite* end = nullptr;
	XMFLOAT2 endScale;
	XMFLOAT4 endColor;


	static Vector2 windowsize;
};
