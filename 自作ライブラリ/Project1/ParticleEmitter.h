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
	static void CreateSmallShock(const Vector3& pos, const float scaleOdds = 1.0f);
	static void CreateHanabi(const Vector3& pos, int rank);
	static void CreateHoming(const Vector3& startPos, Object* targetObj);
	static void CreateTimePlus(const Vector3& pos);
	static void CreateExplosionTogather(const Vector3& startPos, Player* targetObj, const XMFLOAT3& color);
	static  void CreateFeverEffect();
	static void Draw();
	static void Update();
	static void End();

	//ó±éq
	static void CreateSparkEffects(const Vector3& pos, const Vector3& color);
	//éaåÇ
	static void CreateSlashEffects(const Vector3& pos, const float& rotation, const Vector3& color);
	//îgñ‰
	static void CreateShockEffects(const Vector3& pos, const Vector3& color);
	//ó±éq+éaåÇ+îgñ‰
	static void CreateSlashPerfect(const Vector3& pos, const float& rotation, const Vector3& color = { 1,1,1 });

	//åÇîj
	static void CreateRiseEffects(const Vector3& pos, const Vector3& color = { 1,1,1 });

	//ÉuÅ[ÉÅÉâÉì
	static void CreateWindEffects(const Vector3& pos, const Vector3& color = { 1,1,1 });
};

