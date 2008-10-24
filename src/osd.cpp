
#include "osd.h"
#include <GL/gl.h>
#include "game.h"
#include "common.h"

static const char str_banner[] = "bunnykill ultimate!";
static const char str_pressjump[] = "press [jump] to play";
static const char str_quit[] = "press [esc] again to quit";

bool bkOSD::init()
{
	state = gsBanner;
	ratio = anim = 0;
	if (!font.loadfromfreetype ("data/VeraBd.ttf", 64) ) return false;
	messagetime = 0;
	msgratio = 0;
	msg = "";
	shownmsg = "";
	return true;
}

void bkOSD::update (float time)
{
	if (ratio < 0) {
		state = game.gamestate;
	}

	if (game.gamestate != state) ratio -= 2 * time;
	else ratio += 3 * time;
	if (ratio > 1) ratio = 1;

	anim += time;
	if (anim > 1) anim -= 1;

	if (messagetime <= 0) {
		messagetime = 0;
		msg = "";
	}
	if (msg == shownmsg) {
		if (msg.length() ) msgratio += 3 * time;
		if (msgratio > 1) {
			msgratio = 1;
			messagetime -= 3 * time;
		}
	} else {
		msgratio -= 3 * time;
		if (msgratio < 0) {
			msgratio = 0;
			shownmsg = msg;
		}
	}
}

void bkOSD::message (const string& s, float duration)
{
	msg = s;
	messagetime = duration;
}


void bkOSD::draw()
{
	//has to be drawn after the game map (because of blending)
	float f;
	string str;

	glLoadIdentity();

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();
	switch (state) {

	case gsScore:
		glColor4f (0.5, 0.5, 0.5, 0.7*ratio);
		glPushMatrix();
		glTranslatef (0, 0, 0);
		glScalef (0.1, 0.1, 0);
		font.write ("hiscores");
		glPopMatrix();
		break;

	case gsQuit:
		glColor4f (1, 0.1, 0.0, 0.9*ratio);
		glPushMatrix();
		glTranslatef (0.5, 0, 0);
		glScalef (0.02, 0.02, 0);
		glTranslatef (-font.getstrlen (str_quit) / 2, -0.5, 0);
		font.write (str_quit);
		glPopMatrix();
		break;

	case gsResult:
		glColor4f (0, 0, 0, 0.7*ratio);
		glPushMatrix();
		glTranslatef (0.2, -0.1, 0);
		glScalef (0.06, 0.1, 0);
		font.write ( ("Your score: " + itos (game.world.score) ).c_str() );
		glPopMatrix();
		break;

	case gsBanner:
		glColor4f (0.8, anim < 0.5 ? 2*anim : 2* (1 - anim), 0.1, ratio);
		glPushMatrix();
		glTranslatef (0.5, 0, 0);
		glScalef (0.07, 0.2, 0);
		glTranslatef (-font.getstrlen (str_banner) / 2, -0.5, 0);
		font.write (str_banner);
		glPopMatrix();
		glColor4f (0.9, 0.9, 0.9, 0.7*ratio);
		glPushMatrix();
		glTranslatef (0.5, -0.28, 0);
		glScalef (0.04, 0.02, 0);
		glTranslatef (-font.getstrlen (str_pressjump) / 2, 0, 0);
		font.write (str_pressjump);
		glPopMatrix();
		break;

	case gsRunning:
	default:
		glColor4f (0.5, 0.5, 0.5, 1);
		glPushMatrix();
		glTranslatef (0.0, -0.29, 0);
		glPushMatrix();
		glScalef (0.04, 0.02, 0);
		glColor4f (1, 0.4, 0.2, 1);
		font.write ( (" lives: " + itos (game.world.lives >= 0 ?
		                                 game.world.lives : 0) ).c_str() );
		glPopMatrix();
		glPushMatrix();
		glTranslatef (0.33, 0, 0);
		glScalef (0.04, 0.02, 0);
		glColor4f (0.4, 1, 0.2, 1);
		font.write ( (" score: " + itos (game.world.score) ).c_str() );
		glPopMatrix();
		glTranslatef (0.66, 0, 0);
		glScalef (0.04, 0.02, 0);
		glColor4f (0.4, 0.2, 1, 1);
		font.write ( (" level: " + itos (game.world.level) ).c_str() );
		glPopMatrix();
		break;
	}
	glPopMatrix();
	if (msgratio > 0) {
		glPushMatrix();
		glTranslatef (0.5, 0, 0);
		glScalef (0.05, 0.05, 0);
		glColor4f (0, 0, 0, msgratio*0.5);
		glTranslatef (-font.getstrlen (shownmsg.c_str() ) / 2, 0, 0);
		font.write (shownmsg.c_str() );
		glPopMatrix();
	}
	glDisable (GL_BLEND);
}

void bkOSD::release()
{
	font.unload();
}



