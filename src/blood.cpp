
#include "game.h"
#include "blood.h"
#include "exa-image.h"
#include <GL/gl.h>
#include <stdlib.h>
#include <list>
using std::list;
#include "common.h"

static GLuint meattex, bloodtex;

static list<bkMeatPiece> meat;
static list<bkBloodSplat> blood;

#define blood_splat_gen_rate 0.08

bool bkBloodInit()
{
	int x, y, b;
	char*d;
	if (!exaImageReadPNG ("data/tex/bloodsplat.png", &x, &y, &b, &d) ) return false;
	bloodtex = exaImageMakeGLTex (x, y, b, d);
	exaImageFree (d);
	if (!exaImageReadPNG ("data/tex/meat.png", &x, &y, &b, &d) ) return false;
	meattex = exaImageMakeGLTex (x, y, b, d);
	exaImageFree (d);
	return true;
}

void bkBloodRelease()
{
	exaImageDelGLTex (bloodtex);
	exaImageDelGLTex (meattex);
}

bkMeatPiece::bkMeatPiece()
{
	rot = DFRAND * 360;
	splattime = 0;
	timeout = 0.5;
}

void bkMeatPiece::draw()
{
	glPushMatrix();
	glBindTexture (GL_TEXTURE_2D, meattex);
	glTranslatef (pos.x, pos.y, 0);
	glRotatef (rot, 0, 0, 1);
	glColor4f (1, 1, 1, 2*timeout);
	glBegin (GL_QUADS);
	glTexCoord2f (0, 0);
	glVertex2f (-0.007, -0.007);
	glTexCoord2f (1, 0);
	glVertex2f (0.007, -0.007);
	glTexCoord2f (1, 1);
	glVertex2f (0.007, 0.007);
	glTexCoord2f (0, 1);
	glVertex2f (-0.007, 0.007);
	glEnd();
	glPopMatrix();
}

bool bkMeatPiece::update (float time)
{
	splattime += time;
	spd *= powf (0.5, time);
	spd.y -= 0.5 * time;
	pos += time * spd;
	if (spd.length() < 0.02) timeout -= time;
	else while (splattime > 0) {
			splattime -= blood_splat_gen_rate;
			bkBloodAddSplat (pos,
			                 0.7* (spd + (complex (DFRAND, DFRAND) | 0.06) ) );
		}
	if (timeout < 0) return false;
	if (pos.y < 0) return false;
	game.world.map.solvecol (*this);
	return true;
}

bkBloodSplat::bkBloodSplat()
{
	time = 0;
	rot = DFRAND * 360;
	size = FRAND;
	size *= size;
	size = 1 - 0.7 * size;
	splatted = 0;
}

#define splat_dur 5

void bkBloodSplat::draw()
{
	glPushMatrix();
	glBindTexture (GL_TEXTURE_2D, bloodtex);
	glTranslatef (pos.x, pos.y, 0);
	glRotatef (rot, 0, 0, 1);
	glScalef (size, size, 0);
	glColor4f (1, 1, 1, 1 - (time / 5) );
	glBegin (GL_QUADS);
	glTexCoord2f (0, 0);
	glVertex2f (-0.005, -0.005);
	glTexCoord2f (1, 0);
	glVertex2f (0.005, -0.005);
	glTexCoord2f (1, 1);
	glVertex2f (0.005, 0.005);
	glTexCoord2f (0, 1);
	glVertex2f (-0.005, 0.005);
	glEnd();
	glPopMatrix();
}

bool bkBloodSplat::update (float T)
{
	time += T;
	if (time > splat_dur) return false;
	if (pos.x + 0.1 < game.world.camera) return false;
	if (pos.y < 0) return false;
	if (!splatted) {
		spd *= powf (0.2, T);
		spd.y -= 0.5 * T;
		pos += T * spd;
		splatted = game.world.map.solvecol (&pos, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	}
	return true;
}

void bkBloodUpdate (float time, float posdiff)
{

	{
		list<bkMeatPiece>::iterator i;
		for (i = meat.begin();i != meat.end();)
			if (! (i->update (time) ) ) meat.erase (i++);
			else ++i;
	}
	{
		list<bkBloodSplat>::iterator i;
		for (i = blood.begin();i != blood.end();)
			if (! (i->update (time) ) ) blood.erase (i++);
			else ++i;
	}

	if (posdiff > 0) {
		{
			list<bkMeatPiece>::iterator i;
			for (i = meat.begin();i != meat.end();++i) i->pos.x -= posdiff;
		}
		{
			list<bkBloodSplat>::iterator i;
			for (i = blood.begin();i != blood.end();++i) i->pos.x -= posdiff;
		}
	}
}

void bkBloodReset()
{
	meat = list<bkMeatPiece>();
	blood = list<bkBloodSplat>();
}

void bkBloodDraw()
{
	{
		list<bkBloodSplat>::iterator i;
		for (i = blood.begin();i != blood.end();++i) i->draw();
	}
	{
		list<bkMeatPiece>::iterator i;
		for (i = meat.begin();i != meat.end();++i) i->draw();
	}
}

void bkBloodAddSplat (const complex & pos, const complex & spd)
{
	bkBloodSplat bs;
	bs.pos = pos;
	bs.spd = spd;
	blood.push_back (bs);
}

void bkBloodAddMeat (const complex & pos, const complex & spd)
{
	bkMeatPiece mp;
	mp.pos = pos;
	mp.spd = spd;
	meat.push_back (mp);
}

void bkMoveAllBlood (float d)
{
	{
		list<bkBloodSplat>::iterator i;
		for (i = blood.begin();i != blood.end();++i) i->pos.x += d;
	}
	{
		list<bkMeatPiece>::iterator i;
		for (i = meat.begin();i != meat.end();++i) i->pos.x += d;
	}
}

