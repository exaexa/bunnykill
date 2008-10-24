

#include "exa-font.h"
#include "exa-console.h"
#include "exa-base.h"
#include "exa-sound.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define FRAND (rand()/(float)RAND_MAX)
#define DFRAND (1-2*(rand()/(float)RAND_MAX))


exaGLFont font;
exaGLConsole console;
bool showconsole = false;
bool lockfps = true;


void initgl()
{
	glShadeModel (GL_SMOOTH);
	glClearDepth (1.0f);
	glDisable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);
	glFrontFace (GL_CCW);
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho (0, 60, -1, 40, 1, -1);
	glMatrixMode (GL_MODELVIEW);
	glClearColor (0.8f, 0.8f, 0.9f, 0);
}

void drawscene (float dtime)
{
	static float cursorrot = 1;
	cursorrot -= dtime;
	if (cursorrot < 0) cursorrot += 0;
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	glClear (GL_COLOR_BUFFER_BIT);

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (showconsole) console.draw (&font, 40, true, 60, cursorrot, 0, 0, 0);
	else {
		glPushMatrix();
		glColor3f (0.2f, 0.2, 0.25);
		glScalef (2, 2, 1);
		glPushMatrix();
		glTranslatef (0, 1, 0);
		font.write ("Press BACKQUOTE (`) to toggle console, use 'quit'");
		glPopMatrix();
		glPushMatrix();
		font.write ("try 'play bombpl.wav'");
		glPopMatrix();
		glPopMatrix();
		glTranslatef (15, 35, 0);
		console.draw (&font, 5, false, 30, 0, 32, 32, 32);
	}

	glFinish();
	exaglSwapBuffers();
}

bool processconsole()
{
	char cmd[256];
	while (console.getnextcmd (cmd, 256, false) ) {
		if (0 == strcmp (cmd, "quit") ) return false;
		else
			if (0 == strncmp (cmd, "play ", 5) ) {
				exaBuffer*b;
				exaSample*s;
				b = exaSound.createbuffer();
				if (b->loadfile (cmd + 5) ) {
					s = exaSound.createsample (b);
					s->position (vector (DFRAND, DFRAND, 0) );
					s->play();
					s->release();
					b->release();
				}
			} else
				if (0 == strcmp (cmd, "fps_lock") ) lockfps = true;
				else
					if (0 == strcmp (cmd, "fps_unlock") ) lockfps = false;
	}
	return true;
}


int main (int argc, char**argv)
{
	printf ("%d", EXA_F_STEREO16);
	bool quit = false;
	int timer = 0;
	float dtime;
	float rottime = 0;
	float fpstime = 0;
	int nkeys, *keys;
	exaSetParams (1024, 768, 32);
	if (!exaInit() ) return -1;
	//font.loadfromfreetype("consolefont.ttf",256);
	font.loadfromfreetype ("/usr/share/fonts/corefonts/verdana.ttf", 32);
	//font.loadfromfiles("font2_bitmap.raw","font2_descriptor.raw",
	//		128,2048,32);
	console.init (500);
	initgl();

	exaSound.init();
	exaSound.listenerpos (vector (0, 0, 0) );
	exaSound.listenerori (vector (0, 0, -1), vector (0, 1, 0) );

	exaBuffer* b = exaSound.createbuffer();
	b->loadfile ("bombpl.wav");
	exaSample* rotsource = exaSound.createsample (b);
	rotsource->looping (true);
	rotsource->play();

	exaKeyRepeat (0.4, 0.05);
	//console.startlog(stdout, false);

	exaGetElapsedTime();  //reset timer
	while (!quit) {
		dtime = exaGetElapsedTime();
		if (lockfps) while (dtime < 0.009) {
				dtime += exaGetElapsedTime();
				exaUSleep (1000);
			}
		rottime += dtime;
		fpstime += dtime;
		exaUpdate();
		if (!exaIsIconified() ) drawscene (dtime);
		quit = !processconsole();
		if (exaIsKeyHit (EKEY_BACKQUOTE) ) showconsole = !showconsole;
		else
			if (showconsole) {
				nkeys = exaGetKeyTypes (&keys);
				for (--nkeys;nkeys >= 0;--nkeys) {
					console.EKEYinput (keys[nkeys]);
				}
			}
		if (10000 < (timer++) ) {
			exaSound.updateadopted();
			timer = 0;
			console.printf ("fps: %f", 10000 / fpstime);
			fpstime = 0;
		}
		rotsource->position (vector (sinf (rottime), 0, cosf (rottime) ) );
		if (rottime > 2*M_PI) rottime -= (2 * M_PI);
	}
	rotsource->stop();
	rotsource->destroy();
	b->destroy();
	//console.stoplog();
	console.destroy();
	exaSound.shutdown();
	exaShutdown();
	font.unload();
	return 0;
}

