
#include "exa-base.h"
#include "exa-sound.h"
#include <stdio.h>
#include <unistd.h>


int main()
{
	if (!exaSound.init()) return 1;
	exaBuffer* b = exaSound.createbuffer();
	if (!b->loadfile ("bombpl.wav")) printf ("Loading failed\n");
	exaSource* s = exaSound.createsample (b);
	s->play();
	while (s->isplaying()) {
		exaDelay (0.3f);
	}
	s->destroy();
	b->destroy();
	exaSound.shutdown();
	return 0;
}
