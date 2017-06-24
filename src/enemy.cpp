
#define _BK_ENEMY_CPP_

#include "game.h"
#include "enemy.h"
#include "bunnydrawer.h"
#include "common.h"
#include <stdlib.h>


static int enemy_val[] = {
	5, 6, 7, 25, 50, 100, 200
};
#define ENEMYVAL(x) enemy_val[x]

bkEnemy::bkEnemy (int t)
{
	refire = 0;
	type = t;
	//bunny.pos=complex(game.world.camera+1,0.5); //just to be sure
	bunny.pos = complex ( (game.world.map.floor.end() - 1)->x - 0.02 +
	                      game.world.map.pos,
	                      (game.world.map.floor.end() - 1)->y + 0.05);
	switch (type) {
	case 0:
		bunny.blue = 145;
		bunny.green = 208;
		bunny.red = 244;
		break;
	case 1:
		bunny.blue = 128;
		bunny.green = 128;
		bunny.red = 128;
		break;
	case 2:
		bunny.blue = 113;
		bunny.green = 192;
		bunny.red = 255;
		break;
	case 3:
		bunny.blue = 96;
		bunny.red = 255;
		bunny.green = 96;
		break;
	case 4:
		bunny.blue = 128;
		bunny.red = 128;
		bunny.green = 255;
		break;
	case 5:
		bunny.blue = 255;
		bunny.red = 128;
		bunny.green = 192;
		break;
	case 6:  //ultimate black bunny!
		bunny.blue = 64;
		bunny.red = 64;
		bunny.green = 64;
		break;
	}
}

void bkEnemy::draw()
{
	bunny.draw();
}

bool bkEnemy::update (float time)
{
	bool right = false, left = false, jump = false;
	switch (type) {
	case 0:
		left = ! (rand() % 4);
		jump = ! (rand() % 40);
		break;
	case 1:
		left = ! (rand() % 2);
		jump = ! (rand() % 10);
		break;
	case 2:
		left = ! (rand() % 6);
		jump = ! (rand() % 3);
		break;
	case 3:
		left = rand() % 2;
		jump = true;
		break;
	case 4:
		if ( (bunny.pos.y - game.world.bunny.pos.y < 0) &&
		     (fabs (bunny.pos.x - game.world.bunny.pos.x) < 0.04)) {
			if (bunny.pos.x < game.world.bunny.pos.x)
				left = true;
			else right = true;
		} else {
			if (bunny.pos.x > game.world.bunny.pos.x)
				left = rand() % 2;
			else right = rand() % 2;
			if (bunny.pos.y < game.world.bunny.pos.y + 0.1) jump = rand() % 2;
		}
		break;
	case 5:
		if ( (bunny.pos.y - game.world.bunny.pos.y < 0) &&
		     (fabs (bunny.pos.x - game.world.bunny.pos.x) < 0.04)) {
			if (bunny.pos.x < game.world.bunny.pos.x)
				left = true;
			else right = true;
		} else {
			if (bunny.pos.x > game.world.bunny.pos.x)
				left = rand() % 3;
			else right = rand() % 3;
			if (bunny.pos.y < game.world.bunny.pos.y + 0.1) jump = rand() % 3;
		}
		break;
	case 6:
		if ( (bunny.pos.y - game.world.bunny.pos.y < 0) &&
		     (fabs (bunny.pos.x - game.world.bunny.pos.x) < 0.04)) {
			if (bunny.pos.x < game.world.bunny.pos.x)
				left = true;
			else right = true;
		} else {
			if (bunny.pos.x > game.world.bunny.pos.x)
				left = true;
			else right = true;
			if (bunny.pos.y < game.world.bunny.pos.y + 0.1) jump = true;
		}
		break;
	}
	if (!bunny.update (time, right, left, jump)) return false;
	if (bunny.pos.x < -0.01) return false;
	if (bunny.pos.y <= 0) return false;
	return true;
}

static float gen = 0, specgen = 0;

void bkEnemyUpdate (float time, float move)
{
	list<bkEnemy>::iterator i;
	bkBunny &p = game.world.bunny;
	for (i = enemy.begin(); i != enemy.end();) {
		bkBunnySolveCollision (p, i->bunny);
		if (i->bunny.dead) {
			game.world.score += ENEMYVAL (i->type);
			++game.world.kombo;
		}
		if (!i->update (time)) enemy.erase (i++);
		else ++i;
	}

	gen -= move;
	specgen -= move;
	int lev = game.world.level;
	if (lev) {
		lev -= 1;
		while (gen < 0) {
			int t;
			//We have 3 basic enemy types

			//Every series of levels the
			//enemy generator speeds up a little
			gen += FRAND / (1 + (lev / 7));

			switch (lev % 7) {  //generate proper enemy types
			case 0:
				t = 0;
				break;
			case 1:
				t = 1;
				break;
			case 2:
				t = 2;
				break;
			case 3:
				if (rand() % 2) t = 0;
				else t = 1;
				break;
			case 4:
				if (rand() % 2) t = 0;
				else t = 2;
				break;
			case 5:
				if (rand() % 2) t = 1;
				else t = 2;
				break;
			default:
			case 6:
				if (rand() % 3) t = 0;
				else if (rand() % 2) t = 1;
				else t = 2;
				break;
			}

			bkEnemy e (t);
			enemy.push_back (e);
		}

		if (lev / 5) while (specgen < 0) {
				//More advanced enemy types. Should not appear often
				int r, t;
				specgen += 10 * FRAND / (lev / 5);
				r = rand() % 16;
				if (r >> 3) t = 3;
				else if (r >> 2) t = 4;
				else if (r >> 1) t = 5;
				else t = 6;
				bkEnemy e (t);
				enemy.push_back (e);
			} else specgen = 1;

	} else gen = specgen = 1;
}

void bkEnemyDraw()
{
	list<bkEnemy>::iterator i;
	for (i = enemy.begin(); i != enemy.end(); ++i)
		i->draw();
}

void bkEnemyReset()
{
	enemy = list<bkEnemy>();
}

void bkMoveAllEnemies (float diff)
{
	list<bkEnemy>::iterator i;
	for (i = enemy.begin(); i != enemy.end(); ++i)
		i->bunny.pos.x += diff;
}



