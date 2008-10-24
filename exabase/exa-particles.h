
#ifndef _EXA_PARTICLES_H_
#define _EXA_PARTICLES_H_

#include "vector.h"
#include "orientation.h"
#include <GL/gl.h>

#define PI 3.1415926535f
#define HALFPI 1.5707963268f
#define THRHLFPI 4.7123889804f
#define DEG(r) (180.0f*(r)/PI)
#define SIN45 0.70710678118f
#ifndef DFRAND
#define DFRAND ((rand()/(float)RAND_MAX)*2-1)
#endif
#ifndef FRAND
#define FRAND (rand()/(float)RAND_MAX)
#endif


struct exaParticle
{
	vector pos, spd, gravity;
	float size, time, life, pointgravity, friction, rotfactor;
	float r, g, b;
};

struct exaParticleComet
{
	vector pos, spd, gravity;
	float time, life, pointgravity, friction;

	float emission, emtime;
	float emr, emg, emb, emrnd, ems, emsrnd, emspd, emspdrnd, emlife, emlifernd;
	float emf, emfrnd;

	unsigned int explosion;
	float exr, exg, exb, exrnd, exs, exsrnd, exspd, exspdrnd, exspdlowest, exlife, exlifernd;
	float exf, exfrnd;

	unsigned int numchild;
	float childspeed, childspeedrnd, childemrnd, childexrnd, childlife, childlifernd;
};


class exaParticleSystem
{
	exaParticle* p;
	unsigned int curpar;
	unsigned int parnum;
	GLuint model, texture;
	float totaltime, zrotps;
	void deactivateparticle (unsigned int a);
public:
	exaParticleSystem()
	{
		p = NULL;
	}
	~exaParticleSystem()
	{
		if (p) release();
	}
	void init (unsigned int particlecount, float zrotationpersec = 0);
	void release();
	void copyparticle (exaParticle*a);
	void createparticle (const vector position, const vector speed, const vector gravity,
	                     float size, float life, float pointgravity, float friction, float rotfactor,
	                     float r, float g, float b);
	void createline (const exaParticle*a, vector start, vector step, int count);
	void update (float time);
	void draw (const orientation & viewer);
	void clear();

};

class exaParticleCometSystem
{
	exaParticleComet*d;
	unsigned int current;
	unsigned int num;

	void deactivatecomet (unsigned int n);
public:
	exaParticleCometSystem()
	{
		d = NULL;
	}
	~exaParticleCometSystem()
	{
		if (d) release();
	}
	void init (unsigned int numComets);
	void release();
	void update (float time, exaParticleSystem* ps);
	void copycomet (exaParticleComet * pc);



	void createcomet (vector pos, vector spd, vector gravity,

	                  float life, float pointgravity, float friction,

	                  float emission,
	                  float emr, float emg, float emb, float emrnd, float ems, float emsrnd,
	                  float emspd, float emspdrnd, float emlife, float emlifernd,
	                  float emf, float emfrnd,

	                  unsigned int explosion = 0,
	                  float exr = 0, float exg = 0, float exb = 0, float exrnd = 0, float exs = 0,
	                  float exsrnd = 0, float exspd = 0, float exspdrnd = 0, float exspdlowest = 0, float exlife = 0, float exlifernd = 0,
	                  float exf = 0, float exfrnd = 0,

	                  unsigned int numchild = 0,
	                  float childspeed = 0, float childspeedrnd = 0, float childemrnd = 0, float childexrnd = 0,
	                  float childlife = 0, float childlifernd = 0);  //the really most fucking function
};


#endif
