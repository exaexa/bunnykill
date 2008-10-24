
#ifndef _BK_ENEMY_H
#define _BK_ENEMY_H


#include "complex.h"
#include "bunny.h"

void bkEnemyReset();
void bkEnemyDraw();
void bkEnemyUpdate (float time, float move);
void bkMoveAllEnemies (float d);

class bkEnemy
{
public:
	int type;
	float refire;
	bkBunny bunny;

	bkEnemy (int t);
	void draw();
	bool update (float time);
};

#include <list>
using std::list;

#ifndef _BK_ENEMY_CPP_
#define enemy_extern(t,x) extern t
#else
#define enemy_extern(t,x) t x
#endif
enemy_extern (list<bkEnemy> enemy, );

#endif

