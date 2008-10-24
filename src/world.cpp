
#include "game.h"
#include "world.h"
#include "bunnydrawer.h"
#include "blood.h"
#include "enemy.h"
#include "common.h"
#include "hiscore.h"
#include "exa-base.h"

#define STARTING_LIVES 10

bool bkWorld::init()
{
	if (!map.init() ) goto err_quit;
	if (!bkBloodInit() ) goto err_quit;
	if (!bkBunnyDrawerLoad() ) goto err_quit;

	return true;
err_quit:
	release();
	return false;
}

void bkWorld::release()
{
	map.release();
	bkBloodRelease();
	bkBunnyDrawerRelease();
}

void bkWorld::startgame()
{
	timer = 0;
	dist = 0;
	score = level = 0;
	lives = STARTING_LIVES;
	kombo = 0;

	camera = 0;
	map.reset();
	map.set (0, 0);
	bunny.init();
	bunny.pos = complex (0.1, 0.5);
	bunny.spd = complex (0.5, 0);
	bunny.red = bunny.blue = bunny.green = 255;
	bkBloodReset();
	bkEnemyReset();
}

void bkWorld::update (float time)
{
	float posdiff = camera;
	int origscore = score;
	bunny.update (time,
	              game.input.down[keyRight],
	              game.input.down[keyLeft],
	              game.input.down[keyJump]);

	if (bunny.pos.x - camera > 0.5) camera += (bunny.pos.x - 0.5 - camera) * time;
	if (bunny.pos.x - camera > 1) camera = bunny.pos.x - camera - 1;
	camera += time * 0.05;
	posdiff = camera - posdiff;

	map.update (camera);

	bkEnemyUpdate (time, posdiff);
	bkBloodUpdate (time, 0);

	if (bunny.dead) {
		bunny.pos = complex (camera + 0.1, 0.5);
		bunny.spd = complex (0.5, 0);
		--lives;
		bunny.dead = false;
		kombo = 0;
	}

	timer += time;
	dist += posdiff;

	if (bunny.is_on_ground) {
		if (kombo > 1) {
			int sc;
			string t;
			sc = 3 * kombo * kombo * kombo;
			switch (kombo) {
			case 2:
				t = "Double kill bonus!";
				break;
			case 3:
				t = "Multikill bonus!";
				break;
			case 4:
				t = "Monsterkill bonus!";
				break;
			default:
				t = itos (kombo) + "x Kombo Bonus! (" + itos (sc) + ")";
				break;
			}
			score += sc;
			game.osd.message (t, 1);
		}
		kombo = 0;
	}

	if (level) {
		if (dist > 5 + sqrtf (level) ) setlevel (level + 1);
		if (level == 6) if (timer > 3) if (timer < 5) game.osd.message
					("Beware of mutated bunnies");
	} else {
		if (timer < 3) game.osd.message ("jump on (kill) other bunnies");
		else if (timer < 6) game.osd.message ("<-- avoid being there");
		if (timer >= 6) {
			setlevel (1);
		}
	}

	lives += (score / 100) - (origscore / 100);

	if (lives < 0) game.gamestate = gsResult;

	if (camera > 10) move_camera_to_zero();

	if (exaIsKeyDown (EKEY_m) )
		if (exaIsKeyDown (EKEY_l) )
			if (exaIsKeyDown (EKEY_i) )
				++lives;

	if (exaIsKeyHit (EKEY_i) || exaIsKeyHit (EKEY_d) || exaIsKeyHit (EKEY_q) )
		if (exaIsKeyDown (EKEY_q) )
			if (exaIsKeyDown (EKEY_d) )
				if (exaIsKeyDown (EKEY_i) )
					setlevel (level + 1);
}

void bkWorld::setlevel (int l)
{
	level = l;
	timer = 0;
	dist = 0;
	if (level)
		game.osd.message ("Level " + itos (level) );
	else score = 0;
}

void bkWorld::draw()
{
	glPushMatrix();
	//glScalef(0.6,1,1);  //debugging purposes
	glPushMatrix();
	glTranslatef (-camera, -0.25, 0);
	map.draw();
	bkEnemyDraw();
	bunny.draw();
	bkBloodDraw();
	glPopMatrix();
	glDisable (GL_TEXTURE_2D);
	glBegin (GL_QUADS);
	glColor4f (1, 1, 1, 1);
	glVertex2f (0, 0.25);
	glVertex2f (0, -0.25);
	glColor4f (1, 0.5* (1 + sinf (timer*10) ),
	           0.5* (1 + cosf (timer*11) ), 0);
	glVertex2f (0.01, -0.25);
	glVertex2f (0.01, 0.25);
	glEnd();
	glPopMatrix();
}

void bkWorld::move_camera_to_zero()
{
	float diff = -camera;
	camera = 0;
	map.pos += diff;
	bunny.pos.x += diff;
	bkMoveAllEnemies (diff);
	bkMoveAllBlood (diff);
}




