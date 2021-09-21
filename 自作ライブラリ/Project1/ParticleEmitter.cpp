#include "ParticleEmitter.h"
#include"PtrDelete.h"
#include<random>

ParticleManager* ParticleEmitter::particleManager = nullptr;

void ParticleEmitter::Initialize(Camera* camera)
{
	particleManager = ParticleManager::GetInstance();
	particleManager->SetCamera(camera);
	particleManager->Initialize();
}

void ParticleEmitter::CreateExplosion(const Vector3& pos)
{
	for (int i = 0; i < 30; i++)
	{
		Particle* particle = new Particle();
		particle->parameter.position = pos;

		particle->parameter.frame = 0;
		particle->parameter.num_frame = 30;

		particle->parameter.velocity = { 0,0,0 };
		particle->parameter.accel = { (std::rand() % 100 - 50) * 0.01f,(std::rand() % 100 - 50) * 0.01f,(std::rand() % 100 - 50) * 0.01f };

		particle->parameter.scale = 0.5f;
		particle->parameter.s_scale = 0.5f;
		particle->parameter.e_scale = 0.1f;
		//線形補間
		particle->parameter.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;


		particle->parameter.color = { 0,0,0 };
		particle->parameter.s_color = { 1,0,0 };
		particle->parameter.e_color = { 1,0,0 };
		particle->parameter.alpha = 1;

		particle->parameter.isDead = false;

		particleManager->Add(particle, "particle");
	}
}
void ParticleEmitter::CreateSmallShock(const Vector3& pos, const float scaleOdds)
{
	for (int i = 0; i < 3; i++)
	{
		Particle* particle = new Particle();
		particle->parameter.position = pos;

		particle->parameter.frame = 0;
		particle->parameter.num_frame = 10 + 15 * (i % 3);

		particle->parameter.velocity = { 0,0,0 };
		particle->parameter.accel = { 0,0,0 };

		particle->parameter.s_rotation = { 0,0,0 };
		particle->parameter.e_rotation = { 0,0,0 };


		particle->parameter.scale = 0.01f;
		particle->parameter.s_scale = 0.01f;
		particle->parameter.e_scale = 0.7f * scaleOdds;
		//線形補間
		particle->parameter.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;

		particle->parameter.color = { 1,1,1 };
		particle->parameter.s_color = { 0,0,0 };
		particle->parameter.e_color = { 0.3f,0.5f,0.5f };

		particle->parameter.isDead = false;

		particle->parameter.alpha = 1.0f;
		particle->parameter.s_alpha = 1.0f;
		particle->parameter.e_alpha = 0.0f;
		particle->parameter.billboradActive = true;

		particleManager->Add(particle, "shock");
	}
}

void ParticleEmitter::CreateShock(const Vector3& pos)
{
	for (int i = 0; i < 3; i++)
	{
		Particle* particle = new Particle();
		particle->parameter.position = pos;

		particle->parameter.frame = 0;
		particle->parameter.num_frame = 15 + 20 * (i % 3);

		particle->parameter.velocity = { 0,0,0 };
		particle->parameter.accel = { 0,0,0 };

		particle->parameter.s_rotation = { 0,0,0 };
		particle->parameter.e_rotation = { 0,0,0 };


		particle->parameter.scale = 1.0f;
		particle->parameter.s_scale = 1.0f;
		particle->parameter.e_scale = 15.0f;
		//線形補間
		particle->parameter.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;

		particle->parameter.color = { 1,1,1 };
		particle->parameter.s_color = { 0,0,0 };
		particle->parameter.e_color = { 0.3f,0.5f,0.5f };

		particle->parameter.isDead = false;

		particle->parameter.alpha = 1.0f;
		particle->parameter.s_alpha = 1.0f;
		particle->parameter.e_alpha = 0.0f;
		particle->parameter.billboradActive = true;

		particleManager->Add(particle, "shock");
	}
}

void ParticleEmitter::CreateHanabi(const Vector3& pos, int rank)
{
	if (rank < 0 || rank > 4)
		return;
	switch (rank)
	{
	case 0:
		for (int i = 0; i < 1; i++)
		{
			Particle* particle = new Particle();
			particle->parameter.position = pos + Vector3((std::rand() % 1000 * 0.0010f - 0.10f), 0, 0);

			particle->parameter.frame = 0;
			particle->parameter.num_frame = 20;

			particle->parameter.velocity = { (std::rand() % 1000 * 0.001f - 0.5f),3,0 };
			if (particle->parameter.velocity.x > 0)
				particle->parameter.accel = { -(std::rand() % 1000 * 0.000005f),-0.08f,0 };
			else
				particle->parameter.accel = { (std::rand() % 1000 * 0.000005f),-0.08f,0 };


			particle->parameter.scale = (std::rand() % 1000 * 0.005f);
			particle->parameter.scaleVel = 0.01f;
			particle->parameter.scaleAce = (std::rand() % 10000 * 0.000005f);

			particle->parameter.s_color = { 0,0,0 };
			particle->parameter.e_color = { 1,1,1 };
			particle->parameter.isDead = false;


			particleManager->Add(particle, "particle");
		}
		break;
	case 1:
		for (int i = 0; i < 2; i++)
		{
			Particle* particle = new Particle();
			particle->parameter.position = pos + Vector3((std::rand() % 1000 * 0.0010f - 0.10f), 0, 0);

			particle->parameter.frame = 0;
			particle->parameter.num_frame = 20;

			particle->parameter.velocity = { (std::rand() % 1000 * 0.001f - 0.5f),3,0 };
			if (particle->parameter.velocity.x > 0)
				particle->parameter.accel = { -(std::rand() % 1000 * 0.000005f),-0.08f,0 };
			else
				particle->parameter.accel = { (std::rand() % 1000 * 0.000005f),-0.08f,0 };


			particle->parameter.scale = (std::rand() % 1000 * 0.005f);
			particle->parameter.scaleVel = 0.01f;
			particle->parameter.scaleAce = (std::rand() % 10000 * 0.000005f);

			particle->parameter.s_color = { 0,0,0 };
			particle->parameter.e_color = { (std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f) };
			particle->parameter.isDead = false;


			particleManager->Add(particle, "particle");
		}
		break;
	case 2:
		for (int i = 0; i < 3; i++)
		{
			Particle* particle = new Particle();
			particle->parameter.position = pos + Vector3((std::rand() % 1000 * 0.0010f - 0.10f), 0, 0);

			particle->parameter.frame = 0;
			particle->parameter.num_frame = 30;

			particle->parameter.velocity = { 0,3,0 };
			particle->parameter.accel = { (std::rand() % 1000 * 0.00002f - 0.010f),-0.08f,0 };


			particle->parameter.scale = (std::rand() % 1000 * 0.005f);
			particle->parameter.scaleVel = 0.01f;
			particle->parameter.scaleAce = (std::rand() % 10000 * 0.000005f);

			particle->parameter.color = { (std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f) };
			particle->parameter.s_color = { 0,0,0 };
			particle->parameter.e_color = { (std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f) };

			particle->parameter.isDead = false;


			particleManager->Add(particle, "particle");
		}
		break;
	case 3:
		for (int i = 0; i < 5; i++)
		{
			Particle* particle = new Particle();
			particle->parameter.position = pos + Vector3((std::rand() % 1000 * 0.0010f - 0.10f), 0, 0);

			particle->parameter.frame = 0;
			particle->parameter.num_frame = 30;

			//particle->parameter.velocity = { 0,3,0 };
			//particle->parameter.accel = { (std::rand() % 1000 * 0.00002f - 0.010f),-0.08f,0 };
			particle->parameter.velocity = { (std::rand() % 1000 * 0.003f - 1.5f),3,0 };
			particle->parameter.accel.x = ((-particle->parameter.velocity.x) / particle->parameter.num_frame) + (std::rand() % 1000 * 0.00003f - 0.015f);
			particle->parameter.accel.y = -0.08f;


			particle->parameter.scale = (std::rand() % 1000 * 0.005f);
			particle->parameter.scaleVel = 0.01f;
			particle->parameter.scaleAce = (std::rand() % 10000 * 0.000005f);

			particle->parameter.s_color = { 0,0,0 };
			particle->parameter.e_color = { (std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f) };

			particle->parameter.isDead = false;

			particleManager->Add(particle, "particle");
		}
		break;
	case 4:
		int random = std::rand() % 5;
		if (random == 0)
		{
			for (int i = 0; i < 10; i++)
			{
				Particle* particle = new Particle();
				particle->parameter.position = pos;

				particle->parameter.frame = 0;
				particle->parameter.num_frame = 30;

				particle->parameter.velocity = { 0,0,0 };
				particle->parameter.accel = { (std::rand() % 100 - 50) * 0.01f,(std::rand() % 50) * 0.01f,(std::rand() % 100 - 50) * 0.01f };

				particle->parameter.scale = 1.0f;
				particle->parameter.s_scale = 1.0f;
				particle->parameter.e_scale = 5.0f;

				particle->parameter.scaleVel = std::rand() % 1000 * 0.0001f;
				particle->parameter.scaleAce = std::rand() % 1000 * 0.00002f;

				particle->parameter.color = { 0,0,0 };
				particle->parameter.s_color = { 0,0,0 };
				particle->parameter.e_color = { (std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f) };

				particle->parameter.isDead = false;

				particleManager->Add(particle, "particle");
			}
		}
		for (int i = 0; i < 2; i++)
		{
			Particle* particle = new Particle();
			particle->parameter.position = pos + Vector3((std::rand() % 1000 * 0.0010f - 0.10f), 0, 0);

			particle->parameter.frame = 0;
			particle->parameter.num_frame = 20;

			particle->parameter.velocity = { (std::rand() % 1000 * 0.001f - 0.5f),3,0 };
			if (particle->parameter.velocity.x > 0)
				particle->parameter.accel = { -(std::rand() % 1000 * 0.000005f),-0.08f,0 };
			else
				particle->parameter.accel = { (std::rand() % 1000 * 0.000005f),-0.08f,0 };

			particle->parameter.scale = (std::rand() % 1000 * 0.005f);
			particle->parameter.scaleVel = 0.01f;
			particle->parameter.scaleAce = (std::rand() % 10000 * 0.000005f);

			particle->parameter.s_color = { 0,0,0 };
			particle->parameter.e_color = { (std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f) };
			particle->parameter.isDead = false;


			particleManager->Add(particle, "particle");
		}
		for (int i = 0; i < 5; i++)
		{
			Particle* particle = new Particle();
			particle->parameter.position = pos + Vector3((std::rand() % 1000 * 0.0010f - 0.10f), 0, 0);

			particle->parameter.frame = 0;
			particle->parameter.num_frame = 30;

			//particle->parameter.velocity = { 0,3,0 };
			//particle->parameter.accel = { (std::rand() % 1000 * 0.00002f - 0.010f),-0.08f,0 };
			particle->parameter.velocity = { (std::rand() % 1000 * 0.003f - 1.5f),3,0 };
			particle->parameter.accel.x = ((-particle->parameter.velocity.x) / particle->parameter.num_frame) + (std::rand() % 1000 * 0.00003f - 0.015f);
			particle->parameter.accel.y = -0.08f;

			particle->parameter.scale = (std::rand() % 1000 * 0.005f);
			particle->parameter.scaleVel = 0.01f;
			particle->parameter.scaleAce = (std::rand() % 10000 * 0.000005f);

			particle->parameter.s_color = { 0,0,0 };
			particle->parameter.e_color = { (std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f) };

			particle->parameter.isDead = false;

			particleManager->Add(particle, "particle");
		}

		break;

	}
}

void ParticleEmitter::CreateHoming(const Vector3& startPos, Object* targetObj)
{
	for (int i = 0; i < 10; i++)
	{
		Particle* particle = new Particle();
		particle->parameter.gap = { (std::rand() % 200 * 0.001f),(std::rand() % 200 * 0.001f),0 };
		particle->parameter.position = startPos;
		particle->parameter.targetObj = targetObj;
		particle->parameter.type = HOMING;
		particle->parameter.frame = 0;
		particle->parameter.num_frame = 100;

		float velX = (startPos.x - targetObj->GetPosition().x) / particle->parameter.num_frame;
		particle->parameter.velocity = { velX ,0,0 };


		particle->parameter.scale = 0.1f;
		particle->parameter.s_scale = particle->parameter.scale;
		particle->parameter.e_scale = 0.3f;
		particle->parameter.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;

		particle->parameter.s_color = { 1,1,1 };
		particle->parameter.e_color = { (std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f) };
		particle->parameter.isDead = false;


		particleManager->Add(particle, "particle");
	}
}

void ParticleEmitter::CreateTimePlus(const Vector3& pos)
{
	Particle* particle = new Particle();
	particle->parameter.position = pos;
	particle->parameter.startPos = pos;
	particle->parameter.endPos = pos;
	particle->parameter.type = TIMEPLUS;

	particle->parameter.frame = 0;
	particle->parameter.num_frame = 60;

	particle->parameter.velocity = { 0,0,0 };
	//particle->parameter.accel = { (std::rand() % 100 - 50) * 0.01f,(std::rand() % 100 - 50) * 0.01f,(std::rand() % 100 - 50) * 0.01f };

	particle->parameter.scale = 0.3f;
	particle->parameter.s_scale = 0.3f;
	particle->parameter.e_scale = 0.3f;
	//線形補間
	//particle->parameter.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;


	particle->parameter.color = { 0,0,0 };
	particle->parameter.s_color = { 1,1,1 };
	particle->parameter.e_color = { 1,1,1 };
	particle->parameter.alpha = 1;

	particle->parameter.isDead = false;

	particle->parameter.blendType = ALPHA;

	particleManager->Add(particle, "TimePlus");


}

void ParticleEmitter::CreateExplosionTogather(const Vector3& startPos, Player* targetObj, const XMFLOAT3& color)
{
	for (int i = 0; i < 15; i++)
	{
		Particle* particle = new Particle();
		particle->parameter.position = startPos;
		particle->parameter.startPos = startPos;
		particle->parameter.player = targetObj;
		particle->parameter.type = MOVE;
		particle->parameter.easeFrame = 0;
		particle->parameter.maxFrame = 60;

		particle->parameter.frame = 0;
		particle->parameter.num_frame = 60;

		particle->parameter.velocity = { 0,0,0 };
		particle->parameter.accel = { (std::rand() % 100 - 50) * 0.01f,(std::rand() % 100 - 50) * 0.01f,(std::rand() % 100 - 50) * 0.01f };

		particle->parameter.scale = 0.4f;
		particle->parameter.s_scale = particle->parameter.scale;
		particle->parameter.e_scale = 0.2f;
		//線形補間
		particle->parameter.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;


		particle->parameter.color = color;
		particle->parameter.s_color = color;
		particle->parameter.e_color = color;
		particle->parameter.alpha = 1;

		particle->parameter.isDead = false;

		particleManager->Add(particle, "particle2");
	}
}

void ParticleEmitter::CreateFeverEffect()
{
	Particle* particle = new Particle();
	int randVal = rand() % 8;
	int val = randVal;
	if (rand() % 2 == 0)
	{
		particle->parameter.position = { 2.3f - 4.6f / 8 * val,1.3f,-6 };
		particle->parameter.velocity = { (std::rand() % 300 - 150) * 0.0001f,-std::rand() % 100 * 0.00025f - 0.01f,0 };
		particle->parameter.accel = Vector3{ 0,0.001f,0 };
	}
	else
	{
		particle->parameter.position = { 2.3f - 4.6f / 8 * val,-1.3f,-6 };
		particle->parameter.velocity = { (std::rand() % 300 - 150) * 0.0001f,std::rand() % 100 * 0.00025f + 0.01f,0 };
		particle->parameter.accel = { 0,-0.001f,0 };
	}
	particle->parameter.type = FEVER;
	particle->parameter.easeFrame = 0;
	particle->parameter.maxFrame = 60;

	particle->parameter.frame = 0;
	particle->parameter.num_frame = 60;

	particle->parameter.scale = 0.35f;
	particle->parameter.s_scale = particle->parameter.scale;
	particle->parameter.e_scale = 0.75f;
	//線形補間
	particle->parameter.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;


	particle->parameter.color = { 1,0,0 };
	particle->parameter.s_color = { 1,0,0 };
	particle->parameter.e_color = { 1,0,0 };
	particle->parameter.alpha = 0.15f;

	particle->parameter.isDead = false;

	particleManager->Add(particle, "particle3");
}

void ParticleEmitter::Draw()
{
	particleManager->Draw();
}

void ParticleEmitter::Update()
{
	particleManager->Update();
}

void ParticleEmitter::End()
{
	//PtrDelete(particleManager);
}

void ParticleEmitter::CreateSparkEffects(const Vector3& pos)
{
	for (int i = 0; i < 20; i++)
	{
		Particle* particle = new Particle();
		particle->parameter.position = pos;

		particle->parameter.frame = 0;
		particle->parameter.num_frame = 25;

		particle->parameter.velocity = { 0,0,0 };
		particle->parameter.accel = { (std::rand() % 100 - 50) * 0.01f,(std::rand() % 100 - 50) * 0.01f,(std::rand() % 100 - 50) * 0.01f };

		particle->parameter.scale = 1.0f;
		particle->parameter.s_scale = 1.0f;
		particle->parameter.e_scale = 0.1f;
		//線形補間
		particle->parameter.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;


		particle->parameter.color = { 0,0,1 };
		particle->parameter.s_color = { 0,0,1 };
		particle->parameter.e_color = { 0,0,1 };
		particle->parameter.alpha = 1;

		particle->parameter.isDead = false;

		particleManager->Add(particle, "particle");
	}
}

void ParticleEmitter::CreateSlashEffects(const Vector3& pos)
{
	Particle* particle = new Particle();
	particle->parameter.position = pos;

	particle->parameter.frame = 0;
	particle->parameter.num_frame = 15;

	particle->parameter.velocity = { 0,0,0 };
	particle->parameter.accel = { 0,0,0 };

	particle->parameter.rotation = { 0,0,0 };
	particle->parameter.s_rotation = { 0,0,0 };
	particle->parameter.e_rotation = { 0,0,0 };

	particle->parameter.scale = 1.0f;
	particle->parameter.s_scale = 1.0f;
	particle->parameter.e_scale = 8.0f;
	//線形補間
	particle->parameter.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;


	particle->parameter.color = { 0,0,1 };
	particle->parameter.s_color = { 0,0,1 };
	particle->parameter.e_color = { 0,0,1 };
	particle->parameter.alpha = 1;

	particle->parameter.isDead = false;

	particleManager->Add(particle, "line");
}

void ParticleEmitter::CreateShockEffects(const Vector3& pos)
{
	for (int i = 0; i <2; i++)
	{
		Particle* particle = new Particle();
		particle->parameter.position = pos;

		particle->parameter.frame = 0;
		particle->parameter.num_frame = 5 + 20 * (i % 2);

		particle->parameter.velocity = { 0,0,0 };
		particle->parameter.accel = { 0,0,0 };

		particle->parameter.s_rotation = { 0,0,0 };
		particle->parameter.e_rotation = { 0,0,0 };


		particle->parameter.scale = 1.0f;
		particle->parameter.s_scale = 1.0f;
		particle->parameter.e_scale = 4.0f;
		//線形補間
		particle->parameter.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;

		particle->parameter.color = { 0,0,1 };
		particle->parameter.s_color = { 0,0,1 };
		particle->parameter.e_color = { 0,0,1};
		particle->parameter.alpha = 1.0f;

		particle->parameter.isDead = false;

		particleManager->Add(particle, "shock");
	}
}
