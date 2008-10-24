
#include <stdio.h>
#include <string.h>
#include "exa-socket.h"
#include "exa-base.h"

int main()
{
	ipaddress ip;
	exaSocket sock;
	sock.create (EXA_UDP);
	sock.bind (6666);
	sock.setblocking (false);
	//sock.settimeout(10000);
	char a[65536];
	int s;
	while (true) {
		s = sock.recvfrom (a, 65536, ip);
		if (s >= 0) {
			fwrite (a, s, 1, stdout);
			printf ("\n");
		} else exaUSleep (10000);
	}
	return 0;
}


