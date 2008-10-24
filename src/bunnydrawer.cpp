
#include "bunnydrawer.h"
#include <GL/gl.h>
#include "exa-image.h"

static GLuint bunnytex, bunnytex2;

bool bkBunnyDrawerLoad()
{
	int x, y, b;
	char*d;
	if (!exaImageReadPNG ("data/tex/bunny.png", &x, &y, &b, &d) ) return false;
	bunnytex = exaImageMakeGLTex (x, y, b, d);
	exaImageFree (d);
	if (!exaImageReadPNG ("data/tex/bunny-nocolor.png", &x, &y, &b, &d) )
		return false;
	bunnytex2 = exaImageMakeGLTex (x, y, b, d);
	exaImageFree (d);
	return true;
}

void bkBunnyDrawerRelease()
{
	exaImageDelGLTex (bunnytex);
}

static const float _bottom = 13 / (float) 32;
void bkBunnyDraw (int stage, bool reverse, char R, char G, char B)
{
	glEnable (GL_TEXTURE_2D);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture (GL_TEXTURE_2D, bunnytex);
	glColor3ub (R, G, B);
	glPushMatrix();
	glScalef (1.6, 1.6, 0);
	//well, somehow compute tex params and draw a bunny n. stage
	float l = (stage * 19 + 1) / (float) 256;
	float r = (stage * 19 + 20) / (float) 256;
	if (reverse) {
		float t = l;
		l = r;
		r = t;
	}
	glBegin (GL_QUADS);
	glTexCoord2f (l, _bottom);
	glVertex2f (-0.01, -0.003);
	glTexCoord2f (r, _bottom);
	glVertex2f (0.01, -0.003);
	glTexCoord2f (r, 1);
	glVertex2f (0.01, 0.017);
	glTexCoord2f (l, 1);
	glVertex2f (-0.01, 0.017);
	glEnd();
	glBindTexture (GL_TEXTURE_2D, bunnytex2);
	glColor3ub (255, 255, 255);
	glBegin (GL_QUADS);
	glTexCoord2f (l, _bottom);
	glVertex2f (-0.01, -0.003);
	glTexCoord2f (r, _bottom);
	glVertex2f (0.01, -0.003);
	glTexCoord2f (r, 1);
	glVertex2f (0.01, 0.017);
	glTexCoord2f (l, 1);
	glVertex2f (-0.01, 0.017);
	glEnd();
	glPopMatrix();
}



