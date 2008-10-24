
#ifndef _BK_BLOOD_H
#define _BK_BLOOD_H

#include "complex.h"

bool bkBloodInit();
void bkBloodRelease();

void bkBloodUpdate (float time, float posdiff);
void bkBloodReset();
void bkBloodDraw();
void bkBloodAddSplat (const complex & pos, const complex & spd);
void bkBloodAddMeat (const complex & pos, const complex & spd);

void bkMoveAllBlood (float d);


class bkMeatPiece
{
public:
	complex pos, spd;
	float rot;
	float timeout;
	float splattime;
	bkMeatPiece();
	void draw();
	bool update (float time);
};

class bkBloodSplat
{
public:
	complex pos, spd;
	float time, rot, size;
	bool splatted;
	bkBloodSplat();
	void draw();
	bool update (float time);
};

#endif

