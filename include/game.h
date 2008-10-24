
#ifndef _BK_GAME_H
#define _BK_GAME_H

#include "osd.h"
#include "input.h"
#include "sound.h"
#include "world.h"
#include "sky.h"

class bkGame
{
public:
	bkSound sound;
	bkInput input;
	bkOSD osd;
	bkSky sky;
	bkWorld world;

	bool init();
	bool update();
	void shutdown();
	void render();

	int gamestate;

	int screenx, screeny;

	float statetimer;
	const char* playername;
};

#define gsBanner 1
#define gsRunning 2
#define gsScore 3
#define gsQuit 4
#define gsResult 5

#ifndef _BK_GAME_CPP
extern
#endif
	bkGame game;

#endif
