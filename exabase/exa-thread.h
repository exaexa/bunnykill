
#ifndef _EXA_THREAD_H_
#define _EXA_THREAD_H_

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_mutex.h>


class exaThread
{
public:
	SDL_Thread*t;

	exaThread() {
		t = NULL;
	}

	bool start (int (*fn) (void*), void*param);
	void wait (int* status);
	void kill();
};

class exaMutex
{
public:
	SDL_mutex*m;

	exaMutex() {
		m = NULL;
	}
	~exaMutex() {
		if (m) release();
	}

	void init();
	void release();
	bool lock ();
	bool unlock();
	bool isinit() {
		return m ? true : false;
	}
};


#endif

