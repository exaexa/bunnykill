
#ifndef _BK_MAP_H
#define _BK_MAP_H

#include "complex.h"
#include "common.h"
#include "bunny.h"
#include "blood.h"

#include <GL/gl.h>
#include <deque>
using std::deque;

typedef struct {
	complex pos;
	complex size;
}
bkPlatform;

class bkMap
{
public:
	deque<complex> floor;
	storage<bkPlatform> platform;
	float pos, diff;
	float platform_timeout;

	GLuint grass, grasschunk, mud, mudborder, mudchunk;

	bool init();
	void release();

	void set (float position, float diff);
	void draw();
	void update (float pos);
	void reset();
	bool solvecol (bkBunny& bunny);
	bool solvecol (bkMeatPiece& meat);
	bool solvecol (complex* pos, complex* spd, float bbottom, float btop,
	               float bleft, float bright, float reflforce, float minrefl,
	               float topalign,	bool* is_on_ground);
private:
	void compute_new_map();
	void draw_map_rect (complex start, complex size, bool draw_bottom = true);
};




#endif

