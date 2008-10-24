
#ifndef _EXA_SOCKET_H_
#define _EXA_SOCKET_H_

#ifdef WIN32
#	include <winsock2.h>
#	define exasockt SOCKET
#	define exasockaddr_in SOCKADDR_IN
#	define exasockaddr SOCKADDR
#else
#	include <unistd.h>
#	include <netdb.h>
#	include <sys/types.h>
#	include <sys/time.h>
#	include <sys/select.h>
#	include <sys/socket.h>
#	include <netinet/in.h>
#	define exasockt int
#	define exasockaddr_in sockaddr_in
#	define exasockaddr sockaddr
#endif
#include <string.h>

#include <stdint.h>

class ipaddress: public exasockaddr_in
{
public:
	ipaddress()
	{};
	explicit ipaddress (const ipaddress& ipa)
	{
		memcpy (this, &ipa, sizeof (ipaddress) );
	}
	ipaddress (const exasockaddr& ipa)
	{
		memcpy (this, &ipa, sizeof (exasockaddr) );
	}
	ipaddress (const exasockaddr_in& ipa)
	{
		memcpy (this, &ipa, sizeof (sockaddr_in) );
	}
	ipaddress (unsigned char a, unsigned char b, unsigned char c, unsigned char d, uint16_t port);
	ipaddress (uint32_t a, uint16_t port);
	const ipaddress& operator= (const ipaddress& ipa)
	{
		memcpy (this, &ipa, sizeof (ipaddress) );
		return *this;
	}
	operator exasockaddr*()
	{
		return (exasockaddr*) this;
	}
	operator exasockaddr_in*()
	{
		return (exasockaddr_in*) this;
	}
	uint16_t getport()
	{
		return htons (sin_port);
	}
	void setport (uint16_t p)
	{
		sin_port = htons (p);
	}
	uint32_t getaddr()
	{
		return htonl (sin_addr.s_addr);
	}
	void setaddr (uint32_t addr)
	{
		sin_addr.s_addr = htonl (addr);
	}
	void setaddr (unsigned char a, unsigned char b, unsigned char c, unsigned char d)
	{
		sin_addr.s_addr = htonl ( (a << 24) + (b << 16) + (c << 8) + d);
	}

	bool operator== (const ipaddress& ipa) const
	{
		if (sin_addr.s_addr == ipa.sin_addr.s_addr)
			if (sin_port == ipa.sin_port)
				return true;
		return false;
	}

	bool operator< (const ipaddress& ipa) const
	{
		if (sin_addr.s_addr > ipa.sin_addr.s_addr) return false;
		if (sin_addr.s_addr < ipa.sin_addr.s_addr) return true;
		if (sin_port < ipa.sin_port) return true;
		return false;
	}
};

class EXASOCKETINIT
{
public:
#ifdef WIN32
	WSADATA wd;
	EXASOCKETINIT()
	{
		WSAStartup (MAKEWORD (2, 0), &wd);
	}
	~EXASOCKETINIT()
	{
		WSACleanup();
	}
#endif
};

#define EXA_UDP SOCK_DGRAM
#define EXA_TCP SOCK_STREAM

#define exagethostbyname exaSocket::gethostbyname
#define exagethostbyaddr exaSocket::gethostbyaddr

class exaSocket
{
public:
	exasockt sock;
	exaSocket()
	{
		sock = 0;
	}
	exaSocket (exasockt s)
	{
		sock = s;
	}
	~exaSocket()
	{
		if (sock) close();
	}
	operator bool()
	{
		return sock ? true : false;
	}
	operator int()
	{
		return sock;
	}
	bool create (int type);
	bool close();
	bool bind (ipaddress ipa);
	inline bool bind (uint16_t port)
	{
		ipaddress i (0, port);
		return bind (i);
	}
	bool listen (unsigned int nconn = 16);
	bool setblocking (bool);
	bool setbroadcast (bool);
	bool settimeout (unsigned int uS);
	exaSocket accept();

	ipaddress getsockip();
	ipaddress getpeerip();

	int recv (char*b, int maxl);
	int recvfrom (char*b, int maxl, ipaddress & ipa);
	int send (char*b, int l);
	int sendto (char*b, int l, ipaddress ipa);

	static ipaddress gethostbyname (const char* name, uint16_t port = 0);
	static int gethostbyaddr (const ipaddress& addr, char*buf, int buflen = 0);
};

#endif

