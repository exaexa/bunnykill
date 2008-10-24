
#ifndef _BK_OSD_H
#define _BK_OSD_H

#include "exa-font.h"
#include <string>
using std::string;

class bkOSD
{
public:
	exaGLFont font;
	int state;
	float ratio, anim;

	float messagetime, msgratio;
	string msg, shownmsg;

	bool init();
	void update (float time);
	void draw();
	void release();

	void message (const string& m, float duration = 5);
};

#endif

