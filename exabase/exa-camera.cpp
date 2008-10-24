
#include "exa-camera.h"
#include <math.h>

#ifndef _EXA_OMIT_GL_
#	include <GL/gl.h>
#	include <GL/glu.h>
#endif

void exaCamera::update (float time, vector fpos, vector focus, vector up, float ratio)
{
	float r = powf (1 - ratio, time); //r:1-r=focus:orig
	pos = (pos * r) + (fpos * (1 - r) );
	ori.fw = (ori.fw * r) + ( (focus - pos).unitvector() * (1 - r) );
	ori.up = (ori.up * r) + (up * (1 - r) );
	ori.checkangle();
}

void exaCamera::update (float time, vector fpos, orientation focus, float ratio)
{
	float r = powf (1 - ratio, time);
	pos = (pos * r) + (fpos * (1 - r) );
	ori.fw = (ori.fw * r) + (focus.fw * (1 - r) );
	ori.up = (ori.up * r) + (focus.up * (1 - r) );
	ori.checkangle();
}

float exaCamera::getspeed (vector fpos, float ratio)
{
	return (pos -fpos).length() *log (1 - ratio);
}

vector exaCamera::getvectorspeed (vector fpos, float ratio)
{
	return (pos -fpos) *log (1 - ratio);
}


#ifndef _EXA_OMIT_GL_
void exaCamera::glLook()
{
	vector l = ori.fw + pos;
	gluLookAt (pos.x, pos.y, pos.z,
	           l.x, l.y, l.z,
	           ori.up.x, ori.up.y, ori.up.z);
}
#endif
