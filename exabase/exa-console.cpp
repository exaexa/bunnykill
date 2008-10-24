
#include "exa-console.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void exaGLConsole::init (unsigned short int numLines, unsigned short int numCmdRemember)
{
	numdata = numLines;
	numused = numCmdRemember;
	waitingcmds = 0;
	shownUsedCmd = 0;
	curpos = 0;
	cmdqueue = new std::string[16];
	if (numdata > 0) str_data = new std::string[numdata];
	if (numused > 0) usedcmd = new std::string[numused];
	overwrite = false;
	numfilledused = 0;
	scroll = 0;
	isLog = false;
	isinit = true;
}

void exaGLConsole::destroy()
{
	stoplog();
	delete[] cmdqueue;
	delete[] str_data;
	delete[] usedcmd;
	isinit = false;
}

void exaGLConsole::draw (exaGLFont * drwfont, int numLines, bool DrawCmdLine, float maxWidth, float cursorOpacity, unsigned char r, unsigned char g, unsigned char b)
{
	if (!isinit) return;
	int i;
	float t;

	glPushMatrix();
	for (i = scroll; (i < (numLines + scroll) ) && (i < numdata);i++) {

		glPushMatrix();
		t = drwfont->getcoloredstrlen (str_data[i].data() );
		if (t > maxWidth) glScalef (maxWidth / t, 1, 1);
		glColor3ub (r, g, b);
		drwfont->writecolored (str_data[i].data() );
		glPopMatrix();
		glTranslatef (0, 1, 0);  //line up
	}
	glPopMatrix();

	if (DrawCmdLine) {
		if (shownUsedCmd == 0) {
			glPushMatrix();
			glTranslatef (0, -1, 0);
			t = drwfont->getcoloredstrlen (cmdline.data() );
			if (t > maxWidth) glScalef (maxWidth / t, 1, 1);
			glPushMatrix();
			glColor3ub (r, g, b);
			drwfont->writecolored (cmdline.data() );
			glPopMatrix();

			glColor4ub ( (unsigned char) (cursorOpacity*r),
			             (unsigned char) (cursorOpacity*g),
			             (unsigned char) (cursorOpacity*b),
			             (unsigned char) (cursorOpacity*255) );

			if (overwrite) {
				glBegin (GL_QUADS);
				glVertex3f (drwfont->getcoloredstrlen (cmdline.data(), curpos), 0, 0);
				glVertex3f (
				    (curpos == cmdline.length() ) ?
				    (drwfont->getcoloredstrlen (cmdline.data(), curpos) + 0.5f)
				    : (drwfont->getcoloredstrlen (cmdline.data(), curpos + 1) )
				    , 0, 0);
				glVertex3f (
				    (curpos == cmdline.length() ) ?
				    (drwfont->getcoloredstrlen (cmdline.data(), curpos) + 0.5f)
				    : (drwfont->getcoloredstrlen (cmdline.data(), curpos + 1) )
				    , 1, 0);
				glVertex3f (drwfont->getcoloredstrlen (cmdline.data(), curpos), 1, 0);
				glEnd();
			} else {
				glBegin (GL_LINES);
				glVertex3f (drwfont->getcoloredstrlen (cmdline.data(), curpos), 0, 0);
				glVertex3f (
				    (curpos == cmdline.length() ) ?
				    (drwfont->getcoloredstrlen (cmdline.data(), curpos) + 0.5f)
				    : (drwfont->getcoloredstrlen (cmdline.data(), curpos + 1) )
				    , 0, 0);
				glEnd();
			}
			glPopMatrix();
		} else {
			glPushMatrix();
			glTranslatef (0, -1, 0);
			t = drwfont->getcoloredstrlen (usedcmd[shownUsedCmd-1].data() );
			if (t > maxWidth) glScalef (maxWidth / t, 1, 1);
			glColor3f (r, g, b);
			drwfont->writecolored (usedcmd[shownUsedCmd-1].data() );
			glPopMatrix();
		}
	}
}

size_t exaGLConsole::getnextcmd (char *buf, size_t bufsize, bool keep)
{
	if (!isinit) return 0;
	if (waitingcmds == 0) return 0;
	waitingcmds--;
	strncpy (buf, cmdqueue[waitingcmds].data(), bufsize);
	if (!keep) return cmdqueue[waitingcmds].length();
	waitingcmds++;
	return cmdqueue[waitingcmds-1].length();
}

std::string exaGLConsole::getnextcmdstring (bool keep)
{
	if (!isinit) return "";
	if (waitingcmds == 0) return "";
	waitingcmds--;
	if (!keep) return cmdqueue[waitingcmds];
	waitingcmds++;
	return cmdqueue[waitingcmds=1];
}




void exaGLConsole::echo (const char *str)
{
	if (!isinit) return;
	int i;
	for (i = numdata - 1;i > 0;i--) str_data[i].swap (str_data[i-1]);
	str_data[0] = str;
	scroll = 0;
	if (isLog) {
		if (logColors) fwrite (str, strlen (str), 1, log);
		else {
			char*c = (char*) str;

			while (*c != 0) {
				if (*c == '#') {
					if ( (* (c + 1) >= '0') && (* (c + 1) <= '8') ) {
						if ( (* (c + 2) >= '0') && (* (c + 2) <= '8') ) {
							if ( (* (c + 3) >= '0') && (* (c + 3) <= '8') ) {
								c += 4;
							} else {
								fputc ('#', log);
								c++;
							}
						} else {
							fputc ('#', log);
							c++;
						}
					} else {
						fputc ('#', log);
						c++;
						if (*c == '#') c++;
					}
				} else {
					fputc (*c, log);
					c++;
				}

			}



		}

		fputc ('\n', log);
	}
}

void exaGLConsole::printf (const char* format, ... )
{
	/*static char buffer[512];  //this should be enough
	va_list ap;
	va_start(ap,format);
	vsnprintf(buffer,512,format,ap); //upgrade this by man 3 printf example
	va_end(ap);
	echo(buffer);*/
	int n, size = 128;
	char*p, *np;
	va_list ap;

	p = (char*) malloc (size);
	if (!p) return; //prevent segfaults

	//consider using asprintf()!
	while (1) {
		va_start (ap, format);
		n = vsnprintf (p, size, format, ap);
		va_end (ap);

		if (n >= 0 && n < size) {
			echo (p);
			return;  //done here
		}
		if (n >= 0) size = n + 1;
		else size *= 2;
		if ( (np = (char*) realloc (p, size) ) == NULL) {
			free (p);
			return;
		}
		p = np;
	}
}

void exaGLConsole::clear()
{
	if (!isinit) return;
	int i;
	for (i = 0;i < numdata;i++) str_data[i] = "";
	scroll = 0;
}

void exaGLConsole::charinput (char c)
{
	if (!isinit) return;
	static std::string::iterator it;
	if (shownUsedCmd != 0) {
		cmdline = usedcmd[shownUsedCmd-1];
		shownUsedCmd = 0;
		curpos = cmdline.length();
	}
	//if((c<32)||(c>=128)) return;
	//--with __signed char__ is the same as following:
	if (c < 32) return;
	if (overwrite && (curpos < cmdline.length() ) ) cmdline[curpos] = c;
	else {
		it = cmdline.begin();
		it += curpos;
		cmdline.insert (it, 1, c);
	}
	curpos++;
}

void exaGLConsole::EKEYinput (int ekey)
{
	if (!isinit) return;
	int i, j;

	switch (ekey) {
	case EKEY_UP:
		if (shownUsedCmd < numfilledused) shownUsedCmd++;
		break;
	case EKEY_DOWN:
		if (shownUsedCmd > 0) shownUsedCmd--;
		break;
	case EKEY_LEFT:
		if (shownUsedCmd != 0) {
			cmdline = usedcmd[shownUsedCmd-1];
			shownUsedCmd = 0;
			curpos = cmdline.length();
		}
		if (curpos > 0) curpos--;
		break;
	case EKEY_RIGHT:
		if (shownUsedCmd != 0) {
			cmdline = usedcmd[shownUsedCmd-1];
			shownUsedCmd = 0;
			curpos = 0;
		}
		if (curpos < cmdline.length() ) curpos++;
		break;
	case EKEY_PAGEUP:
		scroll += 3;
		if (scroll > numdata) scroll = numdata;
		break;
	case EKEY_PAGEDOWN:
		scroll -= 3;
		if (scroll < 0) scroll = 0;
		break;
	case EKEY_HOME:
		if (shownUsedCmd != 0) {
			cmdline = usedcmd[shownUsedCmd-1];
			shownUsedCmd = 0;
		}
		curpos = 0;
		break;
	case EKEY_END:
		if (shownUsedCmd != 0) {
			cmdline = usedcmd[shownUsedCmd-1];
			shownUsedCmd = 0;
		}
		curpos = cmdline.length();
		break;
	case EKEY_INSERT:
		if (shownUsedCmd != 0) {
			cmdline = usedcmd[shownUsedCmd-1];
			shownUsedCmd = 0;
			curpos = 0;
		}
		overwrite = !overwrite;
		break;
	case EKEY_DELETE:
		if (shownUsedCmd != 0) {
			cmdline = usedcmd[shownUsedCmd-1];
			shownUsedCmd = 0;
			curpos = 0;
		}
		if (curpos < cmdline.length() )
			cmdline.erase (curpos, 1);
		break;
	case EKEY_BACKSPACE:
		if (shownUsedCmd != 0) {
			cmdline = usedcmd[shownUsedCmd-1];
			shownUsedCmd = 0;
			curpos = cmdline.length();
		}
		if (curpos > 0) {
			cmdline.erase (curpos - 1, 1);
			curpos--;
		}
		break;
	case EKEY_RETURN:  //tady v tom je dost bordel --> zprehlednit!


		if (shownUsedCmd != 0) {
			echo ( (">" + usedcmd[shownUsedCmd-1]).data() );

			if (usedcmd[shownUsedCmd-1].length() == 0) {
				curpos = 0;
				break;
			}

			for (i = shownUsedCmd - 1;i > 0;i--) usedcmd[i].swap (usedcmd[i-1]);


			if (waitingcmds >= 16) {
				echo ("!!! console: cmdqueue overflow");
			} else {
				for (i = waitingcmds;i > 0;i--) cmdqueue[i].swap (cmdqueue[i-1]);  //posunout frontu
				cmdqueue[0] = usedcmd[0];
				waitingcmds++;
			}
			shownUsedCmd = 0;
			curpos = 0;
			cmdline = "";
			break;
		}


		curpos = 0;

		if (cmdline.length() == 0) {
			echo (">");
			curpos = 0;
			break;
		}

		echo ( (">" + cmdline).data() );
		if (waitingcmds >= 16) {
			echo ("!!_console: cmdqueue overflow");
			for (i = numused - 1;i > 0;i--) usedcmd[i].swap (usedcmd[i-1]);
			usedcmd[0] = cmdline;
			numfilledused++;
			cmdline = "";
			curpos = 0;
			break;
		}
		for (i = waitingcmds;i > 0;i--) cmdqueue[i].swap (cmdqueue[i-1]);  //posunout frontu
		cmdqueue[0] = cmdline;
		waitingcmds++;
		j = -1;
		for (i = 0;i < numfilledused;i++) {
			if (cmdline.compare (usedcmd[i]) == 0) {
				j = i;
				i = numfilledused;
			}
		}
		if (j > 0) for (i = j;i > 0;i--) usedcmd[i].swap (usedcmd[i-1]);
		else {
			for (i = numused - 1;i > 0;i--) usedcmd[i].swap (usedcmd[i-1]);
			usedcmd[0] = cmdline;
			numfilledused++;
			if (numfilledused > numused) numfilledused = numused;
		}
		cmdline = "";
		curpos = 0;
		break;
	default:
		if (exaIsPrintableEKEY (ekey) ) charinput (exaEKEY2char (ekey) );
		break;
	}
}

void exaGLConsole::startlog (FILE* f, bool colors)
{
	if (!isinit) return;
	stoplog();
	log = f;
	if (!log) return;
	isLog = true;
	logColors = colors;
}

void exaGLConsole::stoplog()
{
	if (!isinit) return;
	isLog = false;
}


