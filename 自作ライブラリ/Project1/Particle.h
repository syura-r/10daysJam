#pragma once
#include"ParticleParameter.h"
#include<d3d12.h>
class Particle
{
protected:
public:
	ParticleParameter parameter;
	OutputData vsParam;
public://ƒƒ“ƒoŠÖ”
	Particle(){};
	virtual ~Particle(){};
	void Dead() { vsParam.isDead = true; }
	bool GetisDead(){ return vsParam.isDead; }


};

