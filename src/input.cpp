
#include "input.h"
#include "exa-base.h"

void bkInput::init()
{
	for (int i = 0;i < _nkeys;++i)
		down[i] = hit[i] = false;
}

void bkInput::update()
{
	down[0] = exaIsKeyDown (EKEY_LEFT);
	down[1] = exaIsKeyDown (EKEY_RIGHT);
	down[2] = exaIsKeyDown (EKEY_UP);
	down[3] = exaIsKeyDown (EKEY_ESCAPE);

	hit[0] = exaIsKeyHit (EKEY_LEFT);
	hit[1] = exaIsKeyHit (EKEY_RIGHT);
	hit[2] = exaIsKeyHit (EKEY_UP);
	hit[3] = exaIsKeyHit (EKEY_ESCAPE);
}

