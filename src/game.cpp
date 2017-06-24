
#define _BK_GAME_CPP
#include "game.h"
#include "exa-base.h"
#include <time.h>
#include <stdlib.h>
#include "hiscore.h"

bool bkGame::init()
{
	exaSetParams (screenx, screeny, 32, 0);
	if (!exaInit()) return false;
	exaSetWindowCaption ("BunnyKill Ultimate");

	if (!sound.init()) {
		exaShutdown();
		return false;
	}

	if (!osd.init()) {
		sound.shutdown();
		exaShutdown();
		return false;
	}

	input.init();

	if (!sky.init()) {
		sound.shutdown();
		osd.release();
		exaShutdown();
		return false;
	}
	sky.set (0);

	if (!world.init()) {
		sky.release();
		sound.shutdown();
		osd.release();
		exaShutdown();
		return false;
	}

	gamestate = gsBanner;
	statetimer = 0;

	bkHSInit();

	//GL init

	glShadeModel (GL_SMOOTH);
	glDisable (GL_DEPTH_TEST);
	glFrontFace (GL_CCW);

	srand (time (NULL));

	return true;
}

bool bkGame::update()
{
	float time = exaGetElapsedTime();
	while (time < 0.02) {
		exaUSleep ( (unsigned int) (1000000 * 0.02 - time));
		time += exaGetElapsedTime();
	}
	statetimer += time;
	exaUpdate();

	sound.update (time);
	input.update();
	osd.update (time);

	if (gamestate != gsRunning) sky.set (sky.pos + time / 10);
	else world.update (time);
	sky.update (time);

	if (!exaIsIconified()) render();

	switch (gamestate) {
	case gsRunning:
		statetimer = 0;
		if (input.hit[keyQuit]) {
			gamestate = gsResult;
		}
		break;

	case gsQuit:
		if (input.hit[keyJump] || (statetimer > 3)) {
			gamestate = gsBanner;
			statetimer = 0;
		}
		if (input.hit[keyQuit]) return false;
		break;

	case gsResult:
		if (input.hit[keyJump] || (statetimer > 10)) {
			gamestate = gsScore;
			statetimer = 0;
		} else if (input.hit[keyQuit]) {
			gamestate = gsQuit;
			statetimer = 0;
		}
		break;

	case gsBanner:
	case gsScore:
	default:
		if ( (statetimer > 10) || input.hit[keyRight] || input.hit[keyLeft]) {
			gamestate = (gamestate == gsBanner) ?
			            gsScore : gsBanner;
			statetimer = 0;
		} else if (input.hit[keyQuit]) {
			gamestate = gsQuit;
			statetimer = 0;
		} else if (input.hit[keyJump]) {
			gamestate = (gamestate == gsBanner) ?
			            gsRunning : gsBanner;
			statetimer = 0;
		}
		if (gamestate == gsRunning) world.startgame();
		break;
	}

	return true;
}

void bkGame::render()
{
	float t = 0.5 * (float) game.screeny / (float) game.screenx;

	glClear (GL_COLOR_BUFFER_BIT);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho (0, 1, -t, t, -1, 1);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	sky.draw();
	if (gamestate == gsRunning) world.draw();
	osd.draw();
	exaglSwapBuffers();
}

void bkGame::shutdown()
{
	//recursive shutdown
	world.release();
	osd.release();
	sky.release();
	sound.shutdown();
	bkHSSave();
	exaShutdown();
}

int main()
{
	game.screenx = 1024;
	game.screeny = 768;
	game.playername = getenv ("USER");
	if (!game.init()) return -1;
	while (game.update());
	game.shutdown();
	return 0;
}



