#pragma once
#include"ParticleParameter.h"
#include<d3d12.h>

class Particle
{
protected:
public:
	ParticleParameter parameter;

public://メンバ関数
	Particle() {};
	virtual ~Particle() {};
	void Dead() { parameter.isDead = true; }
	bool GetisDead() { return parameter.isDead; }


};

