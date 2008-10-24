
#include <stdio.h>
#include "common.h"


string itos (int i)
{
	char* a;
	string s;
	asprintf (&a, "%d", i);
	s = a;
	free (a);
	return s;
}
