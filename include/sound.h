
#ifndef _BK_SOUND_H
#define _BK_SOUND_H

#include "exa-sound.h"

#define sJump 0
#define sFootStamp 1
#define _nsounds 2


class bkSound
{
public:
	bool init();
	void shutdown();
	exaBuffer * sbuf[_nsounds];

	void update (float time);
	float updcounter;

	void play (int, float);
};

#endif
