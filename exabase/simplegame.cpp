

#include "orientation.h"
#include "exa-font.h"
#include "exa-console.h"
#include "exa-base.h"
#include "exa-sound.h"
#include "exa-particles.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef DFRAND
#define DFRAND ((rand()/(float)RAND_MAX)*2-1)
#endif
#ifndef FRAND
#define FRAND (rand()/(float)RAND_MAX)
#endif

exaGLFont font;
exaGLConsole console;
GLuint texture;
exaParticleSystem particlesys;
exaParticleCometSystem cometsys;
bool showconsole = false;
bool lockfps = true;

vector pos (0, 0, -5), spd (0, 0, 0), rot (0, 0, 0);
orientation ori = orientation (0, 0, -1, 0, 1, 0);

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
	gluPerspective (75, 1.33333333, 0.1f, 100);
	glMatrixMode (GL_MODELVIEW);
	glClearColor (0, 0, 0, 0);
}

void drawscene (float dtime)
{
	char buf[64];
	static float cursorrot = 1;
	cursorrot -= dtime;
	if (cursorrot < 0) cursorrot += 1;

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluPerspective (75, 1.33333333, 0.1f, 100);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable (GL_BLEND);
	glBlendFunc (GL_ONE, GL_ONE);

	vector nose (0, 0, 3);
	vector top (0, 0.3, 0);
	vector left (-0.5, 0, 0);
	vector right (0.5, 0, 0);
	glPushMatrix();
	glTranslatef (pos.x, pos.y, pos.z);

	glColor3f (1, 1, 1);
	ori.glrotate();
	glBegin (GL_LINE_STRIP);
	glVertex3fv (nose.v);
	glVertex3fv (top.v);
	glVertex3fv (left.v);
	glVertex3fv (right.v);
	glVertex3fv (top.v);
	glEnd();

	glColor3f (1, 0.6, 0.7);

	glBegin (GL_TRIANGLES);
	glVertex3fv (left.v);
	glVertex3fv (nose.v);
	glVertex3fv (right.v);
	glEnd();
	glPopMatrix();

	glEnable (GL_TEXTURE_2D);
	glBindTexture (GL_TEXTURE_2D, texture);
	particlesys.draw (orientation (0, 0, -1, 0, 1, 0) );
	glDisable (GL_TEXTURE_2D);


	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho (0, 60, -1, 40, 1, -1);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();

	if (showconsole) console.draw (&font, 40, true, 60, cursorrot, 255, 255, 255);
	else {
		glColor3f (0.1, 0.3, 0.6);
		glPushMatrix();
		glTranslatef (45, 0, 0);
		glScalef (2, 2, 0);
		sprintf (buf, "%0.3f/s", !spd);
		font.write (buf);
		glPopMatrix();
		glTranslatef (15, 35, 0);
		console.draw (&font, 5, false, 30, 0, 255, 255, 255);
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
			if (0 == strcmp (cmd, "fps_lock") ) lockfps = true;
			else
				if (0 == strcmp (cmd, "fps_unlock") ) lockfps = false;
	}
	return true;
}

bool processgamelogic (float time)
{
	vector newrot = vector (0, 0, 0);
	static float particletime = 0;
	int mx, my;
	exaGetMouseMove (&mx, &my);
	newrot.x += my * 0.01;
	newrot.y -= mx * 0.01;
	if (exaIsMouseButtonDown (3) ) newrot.z += 0.1;
	if (exaIsMouseButtonDown (1) ) newrot.z -= 0.1;
	if (exaIsKeyDown (EKEY_KP5) ) newrot.x += 0.1;
	if (exaIsKeyDown (EKEY_KP8) ) newrot.x -= 0.1;
	if (exaIsKeyDown (EKEY_KP4) ) newrot.y += 0.1;
	if (exaIsKeyDown (EKEY_KP6) ) newrot.y -= 0.1;
	if (exaIsKeyDown (EKEY_KP9) ) newrot.z += 0.1;
	if (exaIsKeyDown (EKEY_KP7) ) newrot.z -= 0.1;
	rot += ori.relate (newrot);
	ori += rot * time;
	rot *= powf (0.5f, time);
	spd += (exaIsKeyDown (EKEY_SPACE) ? 20 : 0) * ori.fw * time;
	pos += time * spd;
	spd *= powf (0.5f, time);
	particlesys.update (time);
	cometsys.update (time, &particlesys);
	if (exaIsKeyDown (EKEY_SPACE) ) {
		particletime += time;
		while (particletime > 0.01) {
			particlesys.createparticle (pos, -4*ori.fw + vector (DFRAND, DFRAND, DFRAND), vector (0, 0.7, 0),
			                            0.7f, 6, 0, 0.8f, 0.5f,
			                            0.27, 0.27, 0.3);
			particlesys.createparticle (pos, -ori.fw* (3 + FRAND), vector (0, 0, 0),
			                            0.3f, 0.5, 0, 0, 0.5f,
			                            0.85, 0.28, 0.1);
			particletime -= 0.01;
		}
	}
	if (exaIsKeyHit (EKEY_z) )
		cometsys.createcomet (pos + ori.fw*3, ori.fw*50, vector (0, 0, 0), 4, 0, 0,
		                      0.005, 0.3, 1, 0.1, 0, 0.3, 0, 1, 1, 0.3, 0, 0.99, 0);
	if (exaIsKeyHit (EKEY_x) )
		cometsys.createcomet (pos + ori.fw*3, ori.fw*20, vector (0, -10, 0), 1, 0, 0,
		                      0.05, 0.2, 0.5, 1, 0, 0.3, 0, 1, 1, 0.3, 0, 0.99, 0,
		                      100, 1, 0.7, 0.2, 0, 0.75, 0, 0, 20, 0, 1, 0, 0.8, 0,
		                      3, 1, 20, 0.5, 0.5, 1, 0);

	if (exaIsKeyHit (EKEY_F3) ) {
		ori = orientation();
		pos = vector (0, 0, -5);
		spd *= 0;
		rot *= 0;
	}
	if (exaIsKeyDown (EKEY_F2) ) {
		console.printf ("Orientation: fw(%f,%f,%f) up(%f,%f,%f)",
		                ori.fw.x, ori.fw.y, ori.fw.z,
		                ori.up.x, ori.up.y, ori.up.z);
		console.printf ("Position: (%f,%f,%f)",
		                pos.x, pos.y, pos.z);
		console.printf ("Speed: (%f,%f,%f)",
		                spd.x, spd.y, spd.z);
		console.printf ("Rotation: (%f,%f,%f)",
		                rot.x, rot.y, rot.z);
	}

	return true;
}

void preparetexture()
{
	void*b;
	FILE*f;
	b = malloc (4096);
	f = fopen ("particle.gray", "rb");
	if (f) {
		fread (b, 4096, 1, f);
		fclose (f);
	}
	glGenTextures (1, &texture);
	glBindTexture (GL_TEXTURE_2D, texture);
	glTexImage2D (GL_TEXTURE_2D, 0, 1, 64, 64, 0,
	              GL_LUMINANCE, GL_UNSIGNED_BYTE, b);
	glTexParameteri (GL_TEXTURE_2D,
	                 GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D,
	                 GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D,
	                 GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D,
	                 GL_TEXTURE_WRAP_T, GL_CLAMP);
	free (b);
}

void freetexture()
{
	glDeleteTextures (1, &texture);
}

int main (int argc, char**argv)
{
	bool quit = false;
	int timer = 0;
	float dtime;
	float fpstime = 0;
	int nkeys, *keys;
	exaSetParams (1280, 800, 24);
	if (!exaInit() ) return -1;
	font.loadfromfreetype ("consolefont.ttf");
	//font.loadfromfiles("font2_bitmap.raw","font2_descriptor.raw",
	//		128,2048,32);
	preparetexture();
	console.init (500);
	initgl();

	exaSound.init();
	exaSound.listenerpos (vector (0, 0, 0) );
	exaSound.listenerori (vector (0, 0, -1), vector (0, 1, 0) );

	exaKeyRepeat (0.4, 0.05);

	particlesys.init (65536, 0);
	cometsys.init (256);
	//console.startlog(stdout, false);

	exaGetElapsedTime();  //reset timer
	while (!quit) {
		dtime = exaGetElapsedTime();
		if (lockfps) while (dtime < 0.01) {
				exaUSleep (1000);
				dtime += exaGetElapsedTime();
			}
		fpstime += dtime;
		exaUpdate();
		if (!exaIsIconified() ) {
			quit = !processgamelogic (dtime);
			drawscene (dtime);
		}
		quit |= !processconsole();
		if (exaIsKeyHit (EKEY_F1) ) showconsole = !showconsole;
		if (showconsole) {
			nkeys = exaGetKeyTypes (&keys);
			for (--nkeys;nkeys >= 0;--nkeys) {
				console.EKEYinput (keys[nkeys]);
				console.printf ("Keyhit %d", keys[nkeys]);
			}
		}
		if (1000 < (timer++) ) {
			exaSound.updateadopted();
			timer = 0;
			console.printf ("fps: %f", 1000 / fpstime);
			fpstime = 0;
		}
	}
	//console.stoplog();
	console.destroy();
	particlesys.release();
	cometsys.release();
	exaSound.shutdown();
	freetexture();
	font.unload();
	exaShutdown();
	return 0;
}

