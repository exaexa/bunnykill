
#ifndef _BK_INPUT_H
#define _BK_INPUT_H

#define keyLeft 0
#define keyRight 1
#define keyJump 2
#define keyQuit 3
#define _nkeys 4

class bkInput
{
public:
	bool down[_nkeys];
	bool hit[_nkeys];
	void init();
	void update();
};


#endif

