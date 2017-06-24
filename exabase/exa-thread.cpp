

#include "exa-thread.h"



bool exaThread::start (int (*fn) (void*), void*param)
{
	t = SDL_CreateThread (fn, param);
	if (t) return true;
	return false;
}

void exaThread::wait (int*status)
{
	SDL_WaitThread (t, status);
}

void exaThread::kill()
{
	SDL_KillThread (t);
	t = NULL;
}

void exaMutex::init()
{
	if (m) release();
	m = SDL_CreateMutex();
}

void exaMutex::release()
{
	if (!m) return;
	SDL_DestroyMutex (m);
	m = NULL;
}

bool exaMutex::lock ()
{
	if (!m) return false;
	if (SDL_mutexP (m)) return false;
	return true;
}

bool exaMutex::unlock()
{
	if (!m) return false;
	if (SDL_mutexV (m)) return false;
	return true;
}

