
#include "sky.h"
#include "exa-image.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

bool bkSky::init()
{
	int x, y, b;
	char*d;
	if (!exaImageReadPNG ("data/tex/cloud.png", &x, &y, &b, &d)) {
		printf ("couldn't load cloud\n");
		return false;
	}
	tex = exaImageMakeGLTex (x, y, b, d, true, true, false);
	exaImageFree (d);
	pos = 0;

	return true;
}

void bkSky::release()
{
	exaImageDelGLTex (tex);
}

void bkSky::draw()
{
	list<complex>::iterator i;

	glLoadIdentity();
	glDisable (GL_TEXTURE_2D);
	glDisable (GL_BLEND);
	glBegin (GL_QUADS);
	glColor3ub (0xd6, 0xe1, 0xe8);
	glVertex2f (0, -0.25);
	glVertex2f (1, -0.25);
	glColor3ub (0x57, 0xb2, 0xff);
	glColor3ub (0x37, 0x92, 0xe0);
	glVertex2f (1, 0.25);
	glVertex2f (0, 0.25);
	glEnd();
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable (GL_ALPHA_TEST);
	glAlphaFunc (GL_GREATER, 0.1);
	glColor3ub (255, 255, 255);
	glEnable (GL_TEXTURE_2D);
	glBindTexture (GL_TEXTURE_2D, tex);
	for (i = clouds.begin(); i != clouds.end(); ++i) {
		glPushMatrix();
		glTranslatef (i->x - pos, i->y - 0.25, 0);
		glBegin (GL_QUADS);
		glTexCoord2f (0, 0);
		glVertex2f (-0.08, -0.04);
		glTexCoord2f (1, 0);
		glVertex2f (0.08, -0.04);
		glTexCoord2f (1, 1);
		glVertex2f (0.08, 0.04);
		glTexCoord2f (0, 1);
		glVertex2f (-0.08, 0.04);
		glEnd();
		glPopMatrix();
	}
	glDisable (GL_TEXTURE_2D);
	glDisable (GL_ALPHA_TEST);
}

void bkSky::update (float time)
{
	list<complex>::iterator i;
	for (i = clouds.begin(); i != clouds.end(); ++i) {
		if ( ( (i->x - pos) < -0.3) || ( (i->x - pos) > 1.3)) {
			clouds.erase (i);
			--i;
		}
	}
	if (clouds.size() < 8) //CLOUD COUNT!
		if (! (random() % 128))
			clouds.push_back (complex (pos + (1.15), 0.1 + FRAND * 0.3));
}

