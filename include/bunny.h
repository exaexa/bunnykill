
#ifndef _BK_BUNNY_H
#define _BK_BUNNY_H

#include "complex.h"

class bkBunny
{
public:
	complex pos, spd;
	float animtime;
	int animstate;
	bool is_on_ground, forward, dead;
	unsigned char red, green, blue;

	bkBunny() {
		init();
		dead = false;
	}

	void init();
	void die();
	bool update (float time, bool r, bool l, bool jump);
	void draw();

	friend bool bkBunnySolveCollision (bkBunny& a, bkBunny& b);
};


#endif

