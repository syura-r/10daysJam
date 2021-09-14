#pragma once
#include"ParticleManager.h"
#include"Camera.h"
class ParticleEmitter
{
private:
	static ParticleManager* particleManager;
public:
	static void Initialize(Camera* camera);
	static void CreateExplosion(const Vector3& pos);
	static void CreateShock(const Vector3& pos);
	static void CreateHanabi(const Vector3& pos,int rank);

	static void Draw();
	static void Update();
	static void End();
};

