
#include "orientation.h"
#include "triangle.h"
#include "exa-font.h"
#include "exa-console.h"
#include "exa-base.h"
#include "exa-sound.h"
#include "exa-particles.h"
#include "exa-camera.h"
#include "exa-image.h"
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
bool staticcam = false;

vector pos (0, 0, -5), spd (0, 0, 0), rot (0, 0, 0);
orientation ori = orientation (0, 0, -1, 0, 1, 0);
exaCamera camera (pos, ori);
vector gcspd;

exaBuffer *enginesound;
exaBuffer *gunsound;
exaSample *engine;

void initgl()
{
	glShadeModel (GL_SMOOTH);
	glFrontFace (GL_CCW);
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glClearColor (0, 0, 0.1, 0);
	glClearDepth (1.0f);
}

void drawscene (float dtime)
{
	int i, j;
	char buf[64];
	static float cursorrot = 1;
	cursorrot -= dtime;
	if (cursorrot < 0) cursorrot += 1;

	glDepthMask (GL_TRUE);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluPerspective (75, 1.33333333, 0.1f, 500);
	glMatrixMode (GL_MODELVIEW);
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);
	glLoadIdentity();
	camera.glLook();

	vector nose (0, 0, 2);
	vector top (0, 0.3, -1);
	vector left (-0.5, 0, -1);
	vector right (0.5, 0, -1);
	glPushMatrix();
	glDepthMask (GL_TRUE);
	glTranslatef (pos.x, pos.y, pos.z);
	ori.glrotate();

	glColor3f (1, 0.6, 0.7);

	glBegin (GL_TRIANGLES);
	glVertex3fv (left.v);
	glVertex3fv (right.v);
	glVertex3fv (nose.v);

	glVertex3fv (left.v);
	glVertex3fv (top.v);
	glVertex3fv (nose.v);

	glVertex3fv (top.v);
	glVertex3fv (right.v);
	glVertex3fv (nose.v);

	glVertex3fv (left.v);
	glVertex3fv (right.v);
	glVertex3fv (top.v);
	glEnd();

	glColor3f (1, 1, 1);
	glBegin (GL_LINE_STRIP);
	glVertex3fv (nose.v);
	glVertex3fv (top.v);
	glVertex3fv (left.v);
	glVertex3fv (right.v);
	glVertex3fv (top.v);
	glEnd();
	glBegin (GL_LINE_STRIP);
	glVertex3fv (left.v);
	glVertex3fv (nose.v);
	glVertex3fv (right.v);
	glEnd();

	glEnable (GL_BLEND);
	glBlendFunc (GL_ONE, GL_ONE);
	glDepthMask (GL_FALSE);
	glPushMatrix();
	glRotatef (cursorrot * 360, 0, 0, 1);
	for (i = 0; i < 3; ++i) {
		glRotatef (120, 0, 0, 1);
		glBegin (GL_LINE_STRIP);
		glColor3ub (0, 0, 0);
		glVertex3f (0.2, 0, 10);
		glColor3ub (32, 255, 64);
		glVertex3f (0.4, 0, 18);
		glColor3ub (0, 0, 0);
		glVertex3f (0.2, 0, 40);
		glEnd();
	}
	glPopMatrix();
	glPopMatrix();

	glBegin (GL_LINES);
	glColor3f (1, 1, 1);
	glVertex3f (0, 0, 0);
	glVertex3fv (gcspd.v);
	glColor3f (0.2, 0.4, 0.8);
	for (i = -11; i <= 11; i += 2)
		for (j = -11; j <= 11; j += 2) {
			glVertex3f (-110, 10 * i, 10 * j);
			glVertex3f (110, 10 * i, 10 * j);
			glVertex3f (10 * i, -110, 10 * j);
			glVertex3f (10 * i, 110, 10 * j);
			glVertex3f (10 * i, 10 * j, -110);
			glVertex3f (10 * i, 10 * j, 110);
		}
	glEnd();

	glColor3ub (255, 200, 100);
	glBegin (GL_LINE_LOOP);
	glVertex3f (0, 0, -10);
	glVertex3f (10, 0, 0);
	glVertex3f (-10, 0, 0);
	glEnd();


	glEnable (GL_TEXTURE_2D);
	glBindTexture (GL_TEXTURE_2D, texture);
	particlesys.draw (camera.ori);
	glDisable (GL_TEXTURE_2D);


	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho (0, 60, -1, 40, 1, -1);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();

	glDisable (GL_DEPTH_TEST);
	if (showconsole) console.draw (&font, 40, true, 60, cursorrot, 255, 255, 255);
	else {
		glColor3f (0.1, 0.3, 0.6);
		glPushMatrix();
		glTranslatef (45, 0, 0);
		glScalef (2, 2, 0);
		glPushMatrix();
		sprintf (buf, "%0.3f/s", !spd);
		font.write (buf);
		glPopMatrix();
		plane pl (vector (0, 0, 1), 0);
		sprintf (buf, "dist: %0.3f", pl % pos);
		glTranslatef (0, 1, 0);
		glPushMatrix();
		font.write (buf);
		glPopMatrix();
		triangle tr (vector (0, 0, -10), vector (10, 0, 0), vector (-10, 0, 0));
		sprintf (buf, "par: %0.3f", tr.planedist (pos));
		glTranslatef (-10, 1, 0);
		glPushMatrix();
		font.write (buf);
		glPopMatrix();
		sprintf (buf, "cams: %0.4f", gcspd.length());
		glTranslatef (-10, 1, 0);
		glPushMatrix();
		font.write (buf);
		glPopMatrix();
		glPopMatrix();
		glTranslatef (15, 35, 0);
		console.draw (&font, 5, false, 30, 0, 255, 255, 255);
	}
	glDisable (GL_BLEND);

	glFinish();
	exaglSwapBuffers();
}

bool processconsole()
{
	char cmd[256];
	while (console.getnextcmd (cmd, 256, false)) {
		if (0 == strcmp (cmd, "quit")) return false;
		else if (0 == strcmp (cmd, "fps_lock")) lockfps = true;
		else if (0 == strcmp (cmd, "fps_unlock")) lockfps = false;
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
	if (exaIsMouseButtonDown (3)) newrot.z += 15 * time;
	if (exaIsMouseButtonDown (1)) newrot.z -= 15 * time;
	if (exaIsKeyDown (EKEY_KP5)) newrot.x += 15 * time;
	if (exaIsKeyDown (EKEY_KP8)) newrot.x -= 15 * time;
	if (exaIsKeyDown (EKEY_KP4)) newrot.y += 15 * time;
	if (exaIsKeyDown (EKEY_KP6)) newrot.y -= 15 * time;
	if (exaIsKeyDown (EKEY_KP9)) newrot.z += 15 * time;
	if (exaIsKeyDown (EKEY_KP7)) newrot.z -= 15 * time;
	rot += ori.relate (newrot);
	ori += rot * time;
	rot *= powf (0.02f, time);
	if (exaIsKeyDown (EKEY_w)) spd += 30 * ori.fw * time;
	if (exaIsKeyDown (EKEY_s)) spd -= 10 * ori.fw * time;
	if (exaIsKeyDown (EKEY_q)) spd += 10 * ori.up * time;
	if (exaIsKeyDown (EKEY_e)) spd -= 10 * ori.up * time;
	if (exaIsKeyDown (EKEY_a)) spd += 10 * ori.left() * time;
	if (exaIsKeyDown (EKEY_d)) spd += 10 * ori.right() * time;
	pos += time * spd;
	spd *= powf (0.5f, time);
	cometsys.update (time, &particlesys);
	particlesys.update (time);
	float temp = (spd % ori.fw) / 45;
	if (temp > 1) temp = 1;
	engine->pitch (0.7 + 0.6 * temp);
	temp = temp * temp;
	if (particletime > 0.01f) particletime = 0.01f;
	particletime += time;
	float ppt = 0;
	if (exaIsKeyDown (EKEY_w)) {
		ppt = particletime;
		while (ppt > 0.01) {
			particlesys.createparticle (pos - 1.3 * ori.fw,
			                            -ori.fw * 3, vector (0, 0, 0),
			                            0.8, 0.3, 0, 0, 0.5f,
			                            1 - temp, 0.5, temp);
			particlesys.createparticle (pos - 1.3 * ori.fw,
			                            -ori.fw * 3 + (vector (DFRAND, DFRAND, DFRAND) | 0.7),
			                            vector (0, 0, 0),
			                            0.5, 50, 0, 0.7, 0,
			                            0.3, 0.3, 0.3);
			ppt -= 0.01;
		}
	}
	if (exaIsKeyDown (EKEY_s)) {
		ppt = particletime;
		while (ppt >= 0.01) {
			particlesys.createparticle (pos + ori.fw * 2, ori.fw * 15, vector (0, 0, 0),
			                            0.3, 0.3, 0, 0.999, 0.5f,
			                            0.2, 0.5, 1);
			ppt -= 0.01;
		}
	}
	if (exaIsKeyDown (EKEY_a)) {
		ppt = particletime;
		while (ppt >= 0.01) {
			particlesys.createparticle (pos + ori.right() * 0.5, ori.right() * 15, vector (0, 0, 0),
			                            0.3, 0.3, 0, 0.999, 0.5f,
			                            0.2, 0.5, 1);
			ppt -= 0.01;
		}
	}
	if (exaIsKeyDown (EKEY_d)) {
		ppt = particletime;
		while (ppt >= 0.01) {
			particlesys.createparticle (pos + ori.left() * 0.5, ori.left() * 15, vector (0, 0, 0),
			                            0.3, 0.3, 0, 0.999, 0.5f,
			                            0.2, 0.5, 1);
			ppt -= 0.01;
		}
	}
	if (exaIsKeyDown (EKEY_e)) {
		ppt = particletime;
		while (ppt >= 0.01) {
			particlesys.createparticle (pos + ori.up * 0.2, ori.up * 15, vector (0, 0, 0),
			                            0.3, 0.3, 0, 0.999, 0.5f,
			                            0.2, 0.5, 1);
			ppt -= 0.01;
		}
	}
	if (exaIsKeyDown (EKEY_q)) {
		ppt = particletime;
		while (ppt >= 0.01) {
			particlesys.createparticle (pos, -ori.up * 15, vector (0, 0, 0),
			                            0.3, 0.3, 0, 0.999, 0.5f,
			                            0.2, 0.5, 1);
			ppt -= 0.01;
		}
	}
	particletime = ppt;
	if (exaIsKeyHit (EKEY_z))
		cometsys.createcomet (pos + ori.fw * 2, ori.fw * 50, vector (0, 0, 0), 4, 0, 0,
		                      0.005, 0.3, 1, 0.1, 0, 0.3, 0, 1, 1, 0.3, 0, 0.99, 0);
	if (exaIsKeyHit (EKEY_x))
		cometsys.createcomet (pos + ori.fw * 2, ori.fw * 20, vector (0, 0, 0), 1, 0, 0,
		                      0.05, 0.2, 0.5, 1, 0, 0.3, 0, 1, 1, 0.3, 0, 0.99, 0,
		                      100, 1, 0.7, 0.2, 0, 0.75, 0, 0, 20, 0, 1, 0, 0.8, 0,
		                      3, 1, 20, 0.5, 0.5, 1, 0);
	if (exaIsKeyHit (EKEY_c)) {
		cometsys.createcomet (pos + ori.fw * 2, ori.fw * 200, vector (DFRAND, DFRAND, DFRAND) | 200, 1, 0, 0,
		                      0.0003, 0.5, 0.6, 1, 0, 0.2, 0, 0, 0.2, 0.5, 0.2, 0.99, 0);
	}


	if (exaIsKeyHit (EKEY_F3)) {
		ori = orientation();
		pos = vector (0, 0, -5);
		spd *= 0;
		rot *= 0;
	}
	if (exaIsKeyDown (EKEY_F2)) {
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

	float cameraratio;
	vector camerapos;


	if (!staticcam) {
		if (exaIsKeyDown (EKEY_v))
			camera.update (time, camerapos = (pos - ori.fw * 5 + ori.up * 20), pos + 20 * ori.fw, ori.up, cameraratio = 0.9f);
		else if (exaIsKeyDown (EKEY_f))
			camera.update (time, camerapos = (pos + ori.fw * 5), ori, cameraratio = 0.999999f);
		else if (exaIsKeyDown (EKEY_r))
			camera.update (time, camerapos = (pos + ori.fw * 4 + ori.up), ori.backwards(), cameraratio = 0.9999f);
		else
			camera.update (time, camerapos = (pos - ori.fw * 5 + ori.up * 2), ori, cameraratio = 0.9999f);
	} else camera.update (time, camerapos = (camera.pos), pos, camera.ori.up, cameraratio = 0.7f);

	if (exaIsKeyHit (EKEY_F4)) {
		staticcam = !staticcam;
		if (staticcam) console.echo ("Static camera turned ON");
		else console.echo ("Static camera turned OFF");
	}

	//sound stuff!
	float updatecounter = 0;
	updatecounter += time;
	if (updatecounter >= 1) {
		updatecounter -= 1;
		exaSound.updateadopted();
	}

	exaSound.listenerpos (camera.pos);
	exaSound.listenerori (camera.ori.fw, camera.ori.up);
	exaSound.listenerspd (- (gcspd = camera.getvectorspeed (camerapos, cameraratio)));
	engine->position (pos);
	engine->speed (spd);
	static float enginevolume = 0.3f;
	if (exaIsKeyDown (EKEY_w)) enginevolume += 3 * time;
	enginevolume -= time;
	if (enginevolume > 1) enginevolume = 1;
	if (enginevolume < 0.3f) enginevolume = 0.3f;
	engine->volume (enginevolume);

	if (exaIsKeyHit (EKEY_F12)) {
		int x, y, b;
		char*d;
		exaScreenshot (&x, &y, &b, &d);
		exaImageWritePNG ("shot.png", x, y, b, d);
		free (d);
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

void preparesound()
{
	exaSound.init();
	exaSound.soundspeed (50);
	exaSound.dopplerfactor (1);
	enginesound = exaSound.createbuffer();
	gunsound = exaSound.createbuffer();
	enginesound->loadfile ("jetengine.wav");
	engine = exaSound.createsample (enginesound);
	engine->looping (1);
	engine->referencedist (10);
	engine->maxdist (300);
	engine->play();
}

void freetexture()
{
	glDeleteTextures (1, &texture);
}

void freesound()
{
	engine->stop();
	engine->release();
	enginesound->release();
	exaSound.shutdown();
}

int main (int argc, char**argv)
{
	bool quit = false;
	int timer = 0;
	float dtime;
	float fpstime = 0;
	int nkeys, *keys;
	exaSetParams (1024, 768, 32);
	if (!exaInit()) return -1;
	font.loadfromfreetype ("consolefont.ttf", 24);
	//font.loadfromfreetype("/usr/share/fonts/corefonts/verdana.ttf");
	//font.loadfromfiles("font2_bitmap.raw","font2_descriptor.raw",
	//		128,2048,32);
	preparetexture();
	preparesound();
	console.init (500);
	initgl();

	exaKeyRepeat (0.4, 0.05);

	particlesys.init (65536, 0);
	cometsys.init (256);
	//console.startlog(stdout, false);

	exaGetElapsedTime();  //reset timer
	while (!quit) {
		dtime = exaGetElapsedTime();
		if (lockfps) while (dtime < 0.01) {
				exaUSleep (10);
				dtime += exaGetElapsedTime();
			}
		fpstime += dtime;
		exaUpdate();
		if (!exaIsIconified()) {
			quit = !processgamelogic (dtime);
			drawscene (dtime);
		}
		quit |= !processconsole();
		if (exaIsKeyHit (EKEY_F1)) showconsole = !showconsole;
		if (showconsole) {
			nkeys = exaGetKeyTypes (&keys);
			for (--nkeys; nkeys >= 0; --nkeys) {
				console.EKEYinput (keys[nkeys]);
			}
		}
		if (1000 < (timer++)) {
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
	freetexture();
	freesound();
	font.unload();
	exaShutdown();
	return 0;
}

