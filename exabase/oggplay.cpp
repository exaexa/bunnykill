
#include "exa-sound.h"
#include "exa-base.h"
#include <unistd.h>

int main (int argc, char**argv)
{
	if (argc < 2) return 1;

	exaSound.init();

	exaStream s;

	s.load (argv[1]);


	s.play();

	s.volume (0.5);

	while (s.isplaying() || (s.tell() < s.length())) {
		exaUSleep (10000);
		s.update();
		if (!s.isplaying()) s.play();
	}

	s.unload();

	exaSound.shutdown();
	return 0;
}
