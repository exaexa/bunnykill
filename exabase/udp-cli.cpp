
#include <stdio.h>
#include <string.h>
#include "exa-socket.h"


int main()
{
	exaSocket sock;
	ipaddress ip (127, 0, 0, 1, 6666);
	char a[] = "Hello world!";
	sock.create (EXA_UDP);
	sock.sendto (a, strlen (a), ip);
	sock.close();
	return 0;
}
