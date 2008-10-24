
#ifndef _BK_SKY_H
#define _BK_SKY_H

#include <GL/gl.h>
#include <list>
using std::list;
#include "complex.h"

class bkSky
{
public:
	float pos;
	GLuint tex;
	list<complex>clouds;

	bool init();
	void release();

	void draw();
	void update (float time);
	void set (float dest)
	{
		pos = dest;
	}
};


#endif

