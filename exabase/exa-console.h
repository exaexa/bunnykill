#ifndef _EXA_CONSOLE_H_
#define _EXA_CONSOLE_H_

#include "exa-base.h"
#include "exa-font.h"
#include <string>

class exaGLConsole
{
public:
	void stoplog();
	void startlog (FILE*f, bool colors);
	void EKEYinput (int ekey);
	void charinput (char c);
	void clear();
	void clearcmdline()
	{
		cmdline = "";
		curpos = 0;
	}
	void echo (const char*str = "");  //without parameters==free line
	void printf (const char* format, ...);
	size_t getnextcmd (char*buf, size_t bufsize, bool keep);
	std::string getnextcmdstring (bool keep);
	void draw (exaGLFont * drwfont, int numLines, bool DrawCmdLine,
	           float maxWidth, float cursorOpacity, unsigned char r = 255, unsigned char g = 255, unsigned char b = 255);
	void init (unsigned short int scrollback, unsigned short int cmdqueuelen = 10);
	void destroy();

	void unscroll()
	{
		scroll = 0;
	}

	exaGLConsole()
	{
		isinit = false;
	}
	~exaGLConsole()
	{
		if (isinit) destroy();
	}


private:
	bool isinit;
	bool logColors;
	FILE * log;
	bool isLog;
	int scroll;
	int numfilledused;
	bool overwrite;
	unsigned long int curpos;
	int numdata, numused, shownUsedCmd, waitingcmds;
	std::string cmdline;
	std::string *str_data, *usedcmd, *cmdqueue;
};




#endif
