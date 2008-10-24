
#include "exa-socket.h"

#ifndef WIN32
#	include <fcntl.h>
#endif


EXASOCKETINIT _exaSocketInitializator;  //handles win32's wsadata

ipaddress::ipaddress (unsigned char a, unsigned char b, unsigned char c, unsigned char d, uint16_t port)
{
	sin_family = AF_INET;
	sin_port = htons (port);
	sin_addr.s_addr = htonl ( (a << 24) + (b << 16) + (c << 8) + d);
}

ipaddress::ipaddress (uint32_t a, uint16_t port)
{
	sin_family = AF_INET;
	sin_port = htons (port);
	sin_addr.s_addr = htonl (a);
}


bool exaSocket::create (int type)
{
	if (sock) close();
	sock = socket (PF_INET, type, 0);
	if (sock) return true;
	return false;
}

bool exaSocket::close()
{
	if (sock) {
		if (
#ifdef WIN32
		    closesocket (sock)
#else
		    ::close (sock)
#endif
		)
			return false;
		else {
			sock = 0;
			return true;
		}
	}
	return true;
}

bool exaSocket::bind (ipaddress ipa)
{
	if (::bind (sock, (exasockaddr*) ipa, sizeof (exasockaddr) ) ) return false;
	return true;
}

bool exaSocket::listen (unsigned int nconn)
{
	if (::listen (sock, nconn) ) return false;
	return true;
}

bool exaSocket::setblocking (bool block)
{
#ifdef WIN32
	u_long m = block ? 0 : 1;
	if (ioctlsocket (sock, FIONBIO, &m) ) return false;
#else
	if (fcntl (sock, F_SETFL, block ? 0 : O_NONBLOCK) ) return false;
#endif
	return true;
}

bool exaSocket::setbroadcast (bool b)
{
#ifdef WIN32
	char
#else
	int
#endif
	opt = b ? 1 : 0;

	if (!setsockopt (sock, SOL_SOCKET, SO_BROADCAST, &opt, sizeof (opt) ) )
		return true;
	else return false;
}

bool exaSocket::settimeout (unsigned int timeout)
{
	fd_set set;
	FD_ZERO (&set);
	FD_SET (sock, &set);
	timeval t = {
	                timeout, 0
	            };
	if (select (1, &set, NULL, NULL, &t) ) return false;
	return true;
}

exaSocket exaSocket::accept()
{
	int s =::accept (sock, NULL, 0);
	if (s < 0) return exaSocket (0);
	return exaSocket (s);
}

#ifndef WIN32
# define socket_len_t socklen_t
#else
# define socket_len_t int
#endif

ipaddress exaSocket::getsockip()
{
	ipaddress a;
	socket_len_t size = sizeof (a);
	getsockname (sock, (exasockaddr*) a, &size);
	return a;
}

ipaddress exaSocket::getpeerip()
{
	ipaddress a;
	socket_len_t size = sizeof (a);
	getpeername (sock, (exasockaddr*) a, &size);
	return a;
}

int exaSocket::recv (char* buf, int maxl)
{
	return ::recv (sock, buf, maxl, 0);
}

int exaSocket::recvfrom (char*buf, int maxl, ipaddress& ipa)
{
	socket_len_t s = sizeof (ipa);
	return ::recvfrom (sock, buf, maxl, 0, (exasockaddr*) ipa, &s);
}

int exaSocket::send (char*buf, int len)
{
	int s = 0;
	int t = 0;
	if (len == 0) {
		return ::send (sock, buf, len, 0);
	}
	while (s < len) {
		t =::send (sock, buf + s, len - s, 0);
		if (t <= 0) return -1;
		s += t;
	}
	return s;
}

int exaSocket::sendto (char*buf, int len, ipaddress ipa)
{
	int s = 0;
	int t = 0;
	if (len == 0) {
		return ::sendto (sock, buf, len, 0, (exasockaddr*) ipa, sizeof (ipa) );
	}
	while (s < len) {
		t =::sendto (sock, buf + s, len - s, 0, (exasockaddr*) ipa, sizeof (ipa) );
		if (t <= 0) return -1;
		s += t;
	}
	return s;
}

ipaddress exaSocket::gethostbyname (const char* name, uint16_t port)
{
	ipaddress ipa (0, port);
	struct hostent * he;
	he =::gethostbyname (name);
	if (!he) return ipa;

	ipa.sin_addr.s_addr = * ( (uint32_t *) * (he->h_addr_list) );
	//the IP is already served in network byte order
	return ipa;
}

int gethostbyaddr (const ipaddress& ipa, char*buf, int buflen)
{
	struct hostent * he;
	int s;
	he =::gethostbyaddr ( (const char*) & ipa, sizeof (ipa), AF_INET);
	if (!he) return 0;
	if ( (s = strlen (he->h_name) ) >= buflen) {
		strncpy (buf, he->h_name, buflen);
		buf[buflen-1] = 0;
		return buflen;
	}
	strcpy (buf, he->h_name);
	return s;
}


