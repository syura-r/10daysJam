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
		Vector3 position = pos;
		float alpha = 1.0f;

		Vector3 rotation = { 0,0,0 };
		
		particle->vsParam.frame = 0;
		particle->parameter.num_frame = 60;

		Vector3 velocity = { 0,0,0 };
		particle->parameter.accel = { (std::rand()%100 - 50) * 0.01f,(std::rand() % 100 - 50) * 0.01f,(std::rand() % 100 - 50) * 0.01f};

		float scale = 1.0f;
		particle->parameter.s_scale = 1.0f;
		particle->parameter.e_scale = 5.0f;
		//üŒ`•âŠÔ
		particle->vsParam.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale)/ particle->parameter.num_frame ;


		Vector3 color = { 1,1,1 };
		particle->parameter.s_color = { 0,0,0 };
		particle->parameter.e_color = { (std::rand()%100*0.01f),(std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f) };

		particle->vsParam.isDead = false;

		XMMATRIX mat = { position.x,position.y,position.z,0,
						rotation.x,rotation.y,rotation.z,0,
						scale,velocity.x,velocity.y ,velocity.z ,
						color.x,color.y,color.z,alpha };
		
		particle->vsParam.position = position;
		particle->vsParam.rotation = rotation;
		particle->vsParam.velocity = velocity;
		particle->vsParam.scale = scale;
		particle->vsParam.color = { color.x,color.y,color.z,alpha };
		particle->vsParam.billboardActive = 1;

		particleManager->Add(particle,"particle");
	}
}

void ParticleEmitter::CreateShock(const Vector3 & pos)
{
	for (int i = 0; i < 8; i++)
	{
		Particle* particle = new Particle();
		Vector3 position = pos;
		Vector3 rotation = { 0,0,0 };

		particle->vsParam.frame = 0;
		particle->parameter.num_frame = 10 + 10 * (i % 4);

		Vector3 velocity = { 0,0,0 };
		particle->parameter.accel = { 0,0,0 };

		particle->parameter.s_rotation = { 0,0,0 };
		particle->parameter.e_rotation = { 0,0,0 };


		float scale = 1.0f;
		particle->parameter.s_scale = 1.0f;
		particle->parameter.e_scale = 10.0f;
		//üŒ`•âŠÔ
		particle->vsParam.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;

		Vector3 color = { (std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f) };
		particle->parameter.s_color = { 0,0,0 };
		particle->parameter.e_color = { (std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f) };

		particle->vsParam.isDead = false;

		float alpha = 1.0f;

		particle->vsParam.position = position;
		particle->vsParam.rotation = rotation;
		particle->vsParam.velocity = velocity;
		particle->vsParam.scale = scale;
		particle->vsParam.color = { color.x,color.y,color.z,alpha };
		particle->vsParam.billboardActive = 1;
		particleManager->Add(particle, "shock");
	}
}

void ParticleEmitter::CreateHanabi(const Vector3 & pos,int rank)
{
	if (rank < 0 || rank > 4)
		return;
	switch (rank)
	{
	case 0:
		for (int i = 0; i < 1; i++)
		{
			Particle* particle = new Particle();
			Vector3 position = pos + Vector3((std::rand() % 1000 * 0.0010f - 0.10f), 0, 0);

			particle->vsParam.frame = 0;
			particle->parameter.num_frame = 20;

			Vector3 velocity = { (std::rand() % 1000 * 0.001f - 0.5f),3,0 };
			if(velocity.x > 0)
				particle->parameter.accel = { -(std::rand() % 1000 * 0.000005f ),-0.08f,0 };
			else
				particle->parameter.accel = { (std::rand() % 1000 * 0.000005f),-0.08f,0 };


			float scale = (std::rand() % 1000 * 0.005f);
			particle->vsParam.scaleVel = 0.01f;
			particle->parameter.scaleAce = (std::rand() % 10000 * 0.000005f);

			particle->parameter.s_color = { 0,0,0 };
			particle->parameter.e_color = { 1,1,1 };
			particle->vsParam.isDead = false;
			float alpha = 1.0f;
			Vector3 color = { 1,1,1 };
			Vector3 rotation = { 0,0,0 };

			particle->vsParam.billboardActive = true;
			
			particle->vsParam.position = position;
			particle->vsParam.rotation = rotation;
			particle->vsParam.velocity = velocity;
			particle->vsParam.scale = scale;
			particle->vsParam.color = { color.x,color.y,color.z,alpha };
			particle->vsParam.billboardActive = true;

			particleManager->Add(particle, "particle");
		}
		break;
	case 1:
		for (int i = 0; i < 2; i++)
		{
			Particle* particle = new Particle();
			Vector3 position = pos + Vector3((std::rand() % 1000 * 0.0010f - 0.10f), 0, 0);

			particle->vsParam.frame = 0;
			particle->parameter.num_frame = 20;

			Vector3 velocity = { (std::rand() % 1000 * 0.001f - 0.5f),3,0 };
			if (velocity.x > 0)
				particle->parameter.accel = { -(std::rand() % 1000 * 0.000005f),-0.08f,0 };
			else
				particle->parameter.accel = { (std::rand() % 1000 * 0.000005f),-0.08f,0 };


			float scale = (std::rand() % 1000 * 0.005f);
			particle->vsParam.scaleVel = 0.01f;
			particle->parameter.scaleAce = (std::rand() % 10000 * 0.000005f);

			particle->parameter.s_color = { 0,0,0 };
			particle->parameter.e_color = { (std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f) };
			particle->vsParam.isDead = false;
			
			float alpha = 1.0f;
			Vector3 color = { 1,1,1 };
			Vector3 rotation = { 0,0,0 };

			particle->vsParam.billboardActive = true;
			
			particle->vsParam.position = position;
			particle->vsParam.rotation = rotation;
			particle->vsParam.velocity = velocity;
			particle->vsParam.scale = scale;
			particle->vsParam.color = { color.x,color.y,color.z,alpha };
			particle->vsParam.billboardActive = true;

			particleManager->Add(particle, "particle");
		}
		break;
	case 2:
		for (int i = 0; i < 3; i++)
		{
			Particle* particle = new Particle();
			Vector3 position = pos + Vector3((std::rand() % 1000 * 0.0010f - 0.10f), 0, 0);

			particle->vsParam.frame = 0;
			particle->parameter.num_frame = 30;

			Vector3 velocity = { 0,3,0 };
			particle->parameter.accel = { (std::rand() % 1000 * 0.00002f - 0.010f),-0.08f,0 };


			float scale = (std::rand() % 1000 * 0.005f);
			particle->vsParam.scaleVel = 0.01f;
			particle->parameter.scaleAce = (std::rand() % 10000 * 0.000005f);

			Vector3 color = { (std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f) };
			particle->parameter.s_color = { 0,0,0 };
			particle->parameter.e_color = { (std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f) };

			particle->vsParam.isDead = false;

			float alpha = 1.0f;
			Vector3 rotation = { 0,0,0 };

			particle->vsParam.billboardActive = true;
			
			particle->vsParam.position = position;
			particle->vsParam.rotation = rotation;
			particle->vsParam.velocity = velocity;
			particle->vsParam.scale = scale;
			particle->vsParam.color = { color.x,color.y,color.z,alpha };

			particleManager->Add(particle, "particle");
		}
		break;
	case 3:
		for (int i = 0; i < 5; i++)
		{
			Particle* particle = new Particle();
			Vector3 position = pos + Vector3((std::rand() % 1000 * 0.0010f - 0.10f), 0, 0);

			particle->vsParam.frame = 0;
			particle->parameter.num_frame = 30;

			//particle->parameter.velocity = { 0,3,0 };
			//particle->parameter.accel = { (std::rand() % 1000 * 0.00002f - 0.010f),-0.08f,0 };
			Vector3 velocity = { (std::rand() % 1000 * 0.003f - 1.5f),3,0 };
			particle->parameter.accel.x = ((-velocity.x) / particle->parameter.num_frame) + (std::rand() % 1000 * 0.00003f - 0.015f);
			particle->parameter.accel.y = -0.08f;


			float scale = (std::rand() % 1000 * 0.005f);
			particle->vsParam.scaleVel = 0.01f;
			particle->parameter.scaleAce = (std::rand() % 10000 * 0.000005f);

			particle->parameter.s_color = { 0,0,0};
			particle->parameter.e_color = { (std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f) };

			particle->vsParam.isDead = false;
			
			float alpha = 1.0f;
			Vector3 rotation = { 0,0,0 };
			Vector3 color = { 1,1,1 };

			particle->vsParam.billboardActive = true;

			particle->vsParam.position = position;
			particle->vsParam.rotation = rotation;
			particle->vsParam.velocity = velocity;
			particle->vsParam.scale = scale;
			particle->vsParam.color = { color.x,color.y,color.z,alpha };

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
				Vector3 position = pos;

				particle->vsParam.frame = 0;
				particle->parameter.num_frame = 30;

				Vector3 velocity = { 0,0,0 };
				particle->parameter.accel = { (std::rand() % 100 - 50) * 0.01f,(std::rand() % 50) * 0.01f,(std::rand() % 100 - 50) * 0.01f };

				float scale = 1.0f;
				particle->parameter.s_scale = 1.0f;
				particle->parameter.e_scale = 5.0f;
				
				particle->vsParam.scaleVel = std::rand() % 1000 * 0.0001f;
				particle->parameter.scaleAce = std::rand() % 1000 * 0.00002f;

				Vector3 color = { 0,0,0 };
				particle->parameter.s_color = { 0,0,0 };
				particle->parameter.e_color = { (std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f) };

				particle->vsParam.isDead = false;
				float alpha = 1.0f;
				Vector3 rotation = { 0,0,0 };

				particle->vsParam.billboardActive = 1;
				particle->vsParam.position = position;
				particle->vsParam.rotation = rotation;
				particle->vsParam.velocity = velocity;
				particle->vsParam.scale = scale;
			
				particle->vsParam.color = { color.x,color.y,color.z,alpha };

				particleManager->Add(particle, "particle");
			}
		}
		for (int i = 0; i < 2; i++)
		{
			Particle* particle = new Particle();
			Vector3 position = pos + Vector3((std::rand() % 1000 * 0.0010f - 0.10f), 0, 0);

			particle->vsParam.frame = 0;
			particle->parameter.num_frame = 20;

			Vector3 velocity = { (std::rand() % 1000 * 0.001f - 0.5f),3,0 };
			if (velocity.x > 0)
				particle->parameter.accel = { -(std::rand() % 1000 * 0.000005f),-0.08f,0 };
			else
				particle->parameter.accel = { (std::rand() % 1000 * 0.000005f),-0.08f,0 };

			float scale = (std::rand() % 1000 * 0.005f);
			particle->vsParam.scaleVel = 0.01f;
			particle->parameter.scaleAce = (std::rand() % 10000 * 0.000005f);

			particle->parameter.s_color = { 0,0,0 };
			particle->parameter.e_color = { (std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f) };
			particle->vsParam.isDead = false;

			float alpha = 1.0f;
			Vector3 rotation = { 0,0,0 };
			Vector3 color = { 1,1,1 };

			particle->vsParam.billboardActive = true;
			
			particle->vsParam.position = position;
			particle->vsParam.rotation = rotation;
			particle->vsParam.velocity = velocity;
			particle->vsParam.scale = scale;
			particle->vsParam.color = { color.x,color.y,color.z,alpha };

			particleManager->Add(particle, "particle");
		}
		for (int i = 0; i < 5; i++)
		{
			Particle* particle = new Particle();
			Vector3 position = pos + Vector3((std::rand() % 1000 * 0.0010f - 0.10f), 0, 0);

			particle->vsParam.frame = 0;
			particle->parameter.num_frame = 30;

			//particle->parameter.velocity = { 0,3,0 };
			//particle->parameter.accel = { (std::rand() % 1000 * 0.00002f - 0.010f),-0.08f,0 };
			Vector3 velocity = { (std::rand() % 1000 * 0.003f - 1.5f),3,0 };
			particle->parameter.accel.x = ((-velocity.x) / particle->parameter.num_frame) + (std::rand() % 1000 * 0.00003f - 0.015f);
			particle->parameter.accel.y = -0.08f;

			float scale = (std::rand() % 1000 * 0.005f);
			particle->vsParam.scaleVel = 0.01f;
			particle->parameter.scaleAce = (std::rand() % 10000 * 0.000005f);

			particle->parameter.s_color = { 0,0,0 };
			particle->parameter.e_color = { (std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f) };

			particle->vsParam.isDead = false;
			float alpha = 1.0f;
			Vector3 rotation = { 0,0,0 };
			Vector3 color = { 1,1,1 };

			particle->vsParam.billboardActive = true;
			
			particle->vsParam.position = position;
			particle->vsParam.rotation = rotation;
			particle->vsParam.velocity = velocity;
			particle->vsParam.scale = scale;
			particle->vsParam.color = { color.x,color.y,color.z,alpha };

			particleManager->Add(particle, "particle");
		}
		break;
	}
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
	particleManager->End();
}
