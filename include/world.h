
#ifndef _BK_WORLD_H
#define _BK_WORLD_H

#include <list>
using std::list;

#include <map.h>
#include <bunny.h>
#include <blood.h>
#include <enemy.h>

class bkWorld
{
public:
	bkMap map;
	bkBunny bunny;

	float timer, dist;
	int score, level, lives;
	int kombo;

	void startgame();

	float camera;

	bool init();
	void release();

	void update (float time);
	void draw();

	void setlevel (int l);

	void move_camera_to_zero();
};




#endif

