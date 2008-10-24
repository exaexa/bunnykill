
#include "sound.h"

#define SNDFILE(x) ("data/sound/" x ".wav")

static const char* getsoundfilename (int i)
{
	switch (i) {
	case sJump:
		return SNDFILE ("jump");
	case sFootStamp:
		return SNDFILE ("step");
	}
	return NULL;
}

bool bkSound::init()
{
	int i;
	updcounter = 0;
	if (!exaSound.init() ) return false;
	for (i = 0;i < _nsounds;++i) {
		sbuf[i] = exaSound.createbuffer();
		if (! (sbuf[i]) ) return false;
		if (! (sbuf[i]->loadfile (getsoundfilename (i) ) ) )
			return false;
	}
	return true;
}

void bkSound::update (float time)
{
	updcounter += time;
	if (updcounter > 0.1) updcounter -= 0.1, exaSound.updateadopted();
}

void bkSound::shutdown()
{
	int i;
	exaSound.stopadopted();
	for (i = 0;i < _nsounds;++i) {
		sbuf[i]->release();
		delete (sbuf[i]);
	}
	exaSound.shutdown();
}

void bkSound::play (int id, float volume)
{
	exaSample*s;
	if (id >= _nsounds) return;
	if (id < 0) return;
	s = exaSound.createsample (sbuf[id]);
	s->volume (volume);
	s->play();
	s->release(); //and let it playing.
}

