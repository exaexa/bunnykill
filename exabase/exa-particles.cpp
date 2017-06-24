
#include "exa-particles.h"

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

//WARNING, maybe, when creating particle or comet and there's no more space,
//we might want to deactivate a random one.

void exaParticleSystem::init (unsigned int particlecount, float zrotationpersec)
{
	if (p != NULL) release();
	curpar = 0;
	totaltime = 0;
	zrotps = zrotationpersec;
	parnum = ( (particlecount == 0) ? 1 : particlecount);
	p = (exaParticle*) malloc (parnum * sizeof (exaParticle));
	model = glGenLists (1);
	glNewList (model, GL_COMPILE);
	glBegin (GL_QUADS);
	glTexCoord2f (1, 0);
	glVertex3f (-0.5f, -0.5f, 0);
	glTexCoord2f (0, 0);
	glVertex3f (0.5f, -0.5f, 0);
	glTexCoord2f (0, 1);
	glVertex3f (0.5f, 0.5f, 0);
	glTexCoord2f (1, 1);
	glVertex3f (-0.5f, 0.5f, 0);
	glEnd();
	glEndList();
}

void exaParticleSystem::release()
{
	if (p == NULL) return;
	free (p);
	if (glIsList (model)) glDeleteLists (model, 1);
	if (glIsTexture (texture)) glDeleteTextures (1, &texture);
	p = NULL;
}

void exaParticleSystem::copyparticle (exaParticle*a)
{
	unsigned int t;
	if (curpar >= parnum) t = rand() % parnum;
	else t = curpar++;
	if ( ( (a->b == 0) && (a->r == 0) && (a->g == 0)) || (a->life == 0) || (a->size == 0)) return;
	memcpy (p + t, a, sizeof (exaParticle));
	p[t].time = 0;
}

void exaParticleSystem::createparticle (const vector position, const vector speed, const vector gravity,
                                        float size, float life, float pointgravity, float friction, float rotfactor,
                                        float r, float g, float b)
{
	unsigned int t;
	if (curpar >= parnum) t = rand() % parnum;
	else t = curpar++;
	if ( ( (b == 0) && (r == 0) && (g == 0)) || (life == 0) || (size == 0)) return;
	p[t].pos = position;
	p[t].spd = speed;
	p[t].gravity = gravity;
	p[t].pointgravity = pointgravity;
	p[t].friction = (friction > 1) ? 1 : ( (friction < 0) ? 0 : friction);
	p[t].size = size;
	p[t].time = 0.0f;
	p[t].life = life;
	p[t].rotfactor = rotfactor;
	p[t].r = r;
	p[t].g = g;
	p[t].b = b;
}

void exaParticleSystem::createline (const exaParticle*a, const vector start, const vector step, int count)
{
	vector s = start;
	exaParticle p = *a;
	for (; count > 0; --count) {
		p.pos = s;
		copyparticle (&p);
		s += step;
	}
}

void exaParticleSystem::update (float time)
{
	unsigned int i;
	exaParticle*t;

	totaltime += time;

	for (i = 0; i < curpar; i++) {
		t = p + i;
		t->time += time;
		if ( (t->time) >= (t->life)) {
			deactivateparticle (i);
			--i;
		} else {
			if (t->pointgravity == 0) t->spd += t->gravity * time;
			else t->spd += ( (t->gravity - t->pos) | t->pointgravity) * time;
			if (t->friction != 0) t->spd *= powf (1 - (t->friction), time);
			t->pos += t->spd * time;
		}
	}
}

void exaParticleSystem::draw (const orientation & viewer)
{
	unsigned int i;
	float fade;
	GLfloat m[4][4];
	vector *t;
	float tz = totaltime * zrotps;

	viewer.glrotmatrix (m);

	for (i = 0; i < curpar; i++) {
		glPushMatrix();
		t = & (p[i].pos);
		glTranslatef (t->x, t->y, t->z);

		glMultMatrixf ( (GLfloat*) m);

		glRotatef (tz * p[i].rotfactor, 0, 0, 1);

		fade = 1 - (p[i].time / p[i].life);
		glColor3f (p[i].r * fade, p[i].g * fade, p[i].b * fade);

		fade = p[i].size;
		glScalef (fade, fade, fade);
		glCallList (model);

		glPopMatrix();
	}
}

void exaParticleSystem::deactivateparticle (unsigned int a)
{
	if (a >= curpar) return;
	if (a < curpar - 1) memcpy (p + a, p + curpar - 1, sizeof (exaParticle));
	curpar--;
}


void exaParticleCometSystem::init (unsigned int numComets)
{
	num = numComets;
	current = 0;
	d = (exaParticleComet*) malloc (sizeof (exaParticleComet) * num);
}

void exaParticleCometSystem::release()
{
	free (d);
	d = NULL;
}

void exaParticleCometSystem::copycomet (exaParticleComet*a)
{
	if (current >= num) return;
	memcpy (d + current, a, sizeof (exaParticleComet));
	d[current].time = 0;
	current++;
}


void exaParticleCometSystem::createcomet (vector pos, vector spd, vector gravity,

                                          float life, float pointgravity, float friction,

                                          float emission,
                                          float emr, float emg, float emb, float emrnd, float ems, float emsrnd,
                                          float emspd, float emspdrnd, float emlife, float emlifernd,
                                          float emf, float emfrnd,

                                          unsigned int explosion,
                                          float exr, float exg, float exb, float exrnd, float exs,
                                          float exsrnd, float exspd, float exspdrnd, float exspdlowest, float exlife, float exlifernd,
                                          float exf, float exfrnd,

                                          unsigned int numchild,
                                          float childspeed, float childspeedrnd, float childemrnd, float childexrnd,
                                          float childlife, float childlifernd)

{
	if (current >= num) return;

	d[current].time = 0;
	d[current].pos = pos;
	d[current].spd = spd;
	d[current].gravity = gravity;
	d[current].life = life;
	d[current].pointgravity = pointgravity;
	d[current].friction = friction;

	d[current].emission = emission;
	d[current].emtime = 0;

	d[current].emr = emr;
	d[current].emg = emg;
	d[current].emb = emb;
	d[current].emrnd = emrnd;
	d[current].ems = ems;
	d[current].emsrnd = emsrnd;

	d[current].emspd = emspd;
	d[current].emspdrnd = emspdrnd;

	d[current].emlife = emlife;
	d[current].emlifernd = emlifernd;

	d[current].emf = emf;
	d[current].emfrnd = emfrnd;

	d[current].explosion = explosion;

	d[current].exr = exr;
	d[current].exg = exg;
	d[current].exb = exb;
	d[current].exrnd = exrnd;
	d[current].exs = exs;
	d[current].exsrnd = exsrnd;

	d[current].exspd = exspd;
	d[current].exspdrnd = exspdrnd;
	d[current].exspdlowest = exspdlowest;

	d[current].exlife = exlife;
	d[current].exlifernd = exlifernd;

	d[current].exf = exf;
	d[current].exfrnd = exfrnd;

	d[current].numchild = numchild;

	d[current].childspeedrnd = childspeedrnd;
	d[current].childspeed = childspeed;
	d[current].childlifernd = childlifernd;
	d[current].childlife = childlife;
	d[current].childemrnd = childemrnd;
	d[current].childexrnd = childexrnd;

	current++;
}

void exaParticleCometSystem::update (float time, exaParticleSystem* ps)
{
	unsigned int i, j;
	register exaParticleComet*t;


	for (i = 0; i < current; i++) {
		t = d + i;
		t->time += time;
		if ( (t->time) >= (t->life)) {
			//if particle dies
			//make an explosion
			for (j = 0; j < t->explosion; j++) ps->createparticle (t->pos,
				                                                       t->spd * t->exspd + (vector (DFRAND, DFRAND, DFRAND) | (t->exspdlowest + FRAND * t->exspdrnd)),
				                                                       t->gravity,
				                                                       DFRAND * t->exsrnd + t->exs, t->exlife + DFRAND * t->exlifernd, t->pointgravity,
				                                                       t->exf + t->exfrnd * DFRAND, 0,
				                                                       t->exr + DFRAND * t->exrnd,
				                                                       t->exg + DFRAND * t->exrnd,
				                                                       t->exb + DFRAND * t->exrnd);
			//child comets
			for (j = 0; j < t->numchild; j++) createcomet (
				    t->pos, t->spd * t->childspeed + (vector (DFRAND, DFRAND, DFRAND) | t->childspeedrnd),
				    t->gravity, t->life * t->childlife + t->childlifernd * DFRAND, t->pointgravity, t->friction,

				    t->emission, t->emr + DFRAND * t->childemrnd, t->emg + DFRAND * t->childemrnd, t->emb + DFRAND * t->childemrnd,
				    t->emrnd, t->ems, t->emsrnd, t->emspd, t->emspdrnd, t->emlife, t->emlifernd, t->emf, t->emfrnd,
				    t->explosion, t->exr + DFRAND * t->childexrnd, t->exg + DFRAND * t->childexrnd, t->exb + DFRAND * t->childexrnd,
				    t->exrnd, t->exs, t->exsrnd, t->exspd, t->exspdrnd, t->exspdlowest, t->exlife, t->exlifernd,
				    t->exf, t->exfrnd, (t->numchild > 0) ? (t->numchild - 1) : 0,
				    t->childspeed, t->childspeedrnd, t->childemrnd, t->childexrnd, t->childlife, t->childlifernd);
			deactivatecomet (i);
			--i; //as we copy a new particle here, we must process it once more.
		} else { //comet doesn't die
			if (t->pointgravity == 0)
				t->spd += t->gravity * time;
			else
				t->spd += ( ( (t->gravity) - (t->pos)) | t->pointgravity) * time;
			if (t->friction != 0) t->spd *= powf (1 - (t->friction), time);
			exaParticle p;
			p.pos = t->pos;
			t->pos += t->spd * time;
			t->emtime += time;
			p.gravity = vector (0, 0, 0);
			p.pointgravity = 0;
			p.rotfactor = 0;
			int np = (int) (t->emtime / t->emission);
			//ps->createline(&p,lpos,t->emission*t->spd,np);
			t->emtime -= np * t->emission;
			for (; np > 0; --np) {
				p.spd = t->spd * t->emspd + (vector (DFRAND, DFRAND, DFRAND) | t->emspdrnd);
				p.size = t->ems + DFRAND * t->emsrnd;
				p.life = t->emlife + DFRAND * t->emlifernd;
				p.friction = t->emf + t->emfrnd;
				p.r = t->emr + DFRAND * t->emrnd;
				p.g = t->emg + DFRAND * t->emrnd;
				p.b = t->emb + DFRAND * t->emrnd;
				ps->copyparticle (&p);
				p.pos += t->emission * t->spd;
			}
		}
	}
}

void exaParticleCometSystem::deactivatecomet (unsigned int a)
{
	if (a >= current) return;
	if (a < current - 1) memcpy (d + a, d + current - 1, sizeof (exaParticleComet));
	current--;
}
