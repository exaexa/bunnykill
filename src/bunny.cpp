
#include "game.h"
#include "bunny.h"
#include <math.h>
#include "bunnydrawer.h"
#include "blood.h"
#include "common.h"

#define bunny_accel 1
#define jump_accel 0.9
#define gravity 1.6
#define jump_strength 0.4

void bkBunny::init()
{
	animstate = 0;
	is_on_ground = false;
	forward = false;
}

bool bkBunny::update (float time, bool right, bool left, bool jump)
{

	if (pos.x < game.world.camera) {
		die();
		return false;
	}
	if (right) spd.x += time * bunny_accel;
	if (left) spd.x -= time * bunny_accel;
	if (jump) if (spd.y > 0) spd.y += time * jump_accel;
	if (is_on_ground) {
		if (jump) spd.y += jump_strength;
	} else
		spd.y -= time * gravity;

	if (is_on_ground)
		spd.x *= powf (0.03, time);
	else {
		if (spd.y < 0) spd.y *= powf (0.3, time);
		spd.x *= powf (0.1, time);
	}
	pos += time * spd;
	if (pos.y < 0) {
		pos.y = 0;
		spd.y = 0;
	}

	game.world.map.solvecol (*this);

	if (is_on_ground) {
		if ( (fabs (spd.x) <= 0.05) && !left && !right ) animstate = 0;
		else {
			animtime += time;
			if (animtime > 0.1) {
				animtime -= 0.1;
				animstate += 1;
				if (animstate > 3) animstate = 1;
			}
		}
	} else {
		if (fabs (spd.y) < 0.1) animstate = 5;
		else if (spd.y > 0) animstate = 4;
		else {
			animtime += time;
			if ( (animstate < 6) || (animstate > 7) ) {
				animstate = 6;
				animtime = 0;
			}
			if (animtime > 0.1) {
				animtime -= 0.1;
				animstate += 1;
				if (animstate > 7) animstate = 6;
			}
		}
	}

	if (spd.x != 0) forward = spd.x > 0;

	return !dead;
}

void bkBunny::draw()
{
	glPushMatrix();
	glTranslatef (pos.x, pos.y, 0);
	bkBunnyDraw (animstate, !forward, red, green, blue);
	glPopMatrix();
}
void bkBunny::die()
{
	int i;
	complex p = pos + complex (0, 0.0272);
	for (i = 0;i < 10;++i) bkBloodAddMeat
		(pos, complex (DFRAND, DFRAND) | (FRAND*FRAND) );
	for (i = 0;i < 20;++i) bkBloodAddSplat
		(pos, complex (0, 0.5) + 3*spd + complex (DFRAND, DFRAND) | (0.7*FRAND) );
	dead = true;
}

#define clamp_neg(x) if(x<0)x=0

bool bkBunnySolveCollision (bkBunny& a, bkBunny& b)
{
	complex d = a.pos - b.pos;
	float l;
	if ( (l = d.length() ) >= 0.025) return false;
	if (l < 0.0001) return false;
	d = d.unit();

	float s = (b.spd % d) - (a.spd % d);
	if (s < 0) return false;
	d *= s;

	a.spd += d;
	b.spd -= d;

	if (2*fabs (d.y) > fabs (d.x) ) { //die
		if (d.y > 0) {
			b.die();
			clamp_neg (a.spd.y);
			a.spd.y += jump_strength;
		} else {
			a.die();
			clamp_neg (b.spd.y);
			b.spd.y += jump_strength;
		}
	}

	return true;
}

