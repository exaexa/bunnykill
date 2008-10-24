
#include "exa-thread.h"
#include "exa-base.h"

#include <stdio.h>
#include <unistd.h>

int printer (void*param)
{
	while (true) {
		printf ( (const char*) param);
		printf ("\n");
		exaUSleep (1000000);
	}
}


int main()
{
	exaThread p[3];
	p[0].start (printer, (void*) "a");
	p[1].start (printer, (void*) "b");
	p[2].start (printer, (void*) "c");
	while (true) exaUSleep (1000);
}
