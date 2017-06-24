
#define _EXA_SOUND_CPP_
#include "exa-sound.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void checkALError()
{
	int e;
	e = alGetError();
	if (e) printf ("al error 0x%x \n", e);
	e = alutGetError();
	if (e) printf ("alut error 0x%x: %s\n", e, alutGetErrorString (e));
}

bool exaSoundSystem::init()
{
	if (isinit == true) return isinit;

	if (alutInit (NULL, NULL) != AL_TRUE) {
		printf ("error loading alut: %x \n", alutGetError());
		return false;
	}
	isinit = true;
	return isinit;
}

void exaSoundSystem::shutdown()
{
	if (isinit == false) return;
	alutExit();
	isinit = false;
}

exaBuffer* exaSoundSystem::createbuffer()
{
	return new exaBuffer;
}

exaSample* exaSoundSystem::createsample (exaBuffer* b)
{
	return new exaSample (b);
}

exaStream* exaSoundSystem::createstream()
{
	return new exaStream;
}

void exaSoundSystem::listenerori (vector forward, vector top)
{
	float v[6];
	memcpy (v, forward.v, sizeof (float) * 3);
	memcpy (v + 3, top.v, sizeof (float) * 3);
	alListenerfv (AL_ORIENTATION, v);
}

void exaSoundSystem::adoptsample (exaSample* s)
{
	s->looping (false);
	samples.insert (s);
}

void exaSoundSystem::adoptbuffer (exaBuffer* b)
{
	buffers.insert (b);
}

void exaSoundSystem::stopadopted()
{
	set<exaSample*>::iterator i = samples.begin();
	while (i != samples.end()) {
		(*i)->stop();
		i++;
	}
}

void exaSoundSystem::updateadopted()
{
	set<exaSample*>::iterator i = samples.begin();
	while (i != samples.end()) {
		if ( ( (*i)->isplaying()) % 2) i++;
		else {
			(*i)->stop();
			(*i)->destroy();
			samples.erase (i);
		}
	}

	set<exaBuffer*>::iterator j = buffers.begin();
	while (j != buffers.end()) {
		if (alIsBuffer ( (*j)->buffer))
			alDeleteBuffers (1, & ( (*j)->buffer));
		if (!alIsBuffer ( (*j)->buffer)) {
			//if can't be deleted then this one is still playing
			(*j)->destroy();
			buffers.erase (j);
		} else
			j++;
	}
	checkALError();
}




exaBuffer::exaBuffer()
{
	alGenBuffers (1, &buffer);
}

exaBuffer::~exaBuffer()
{
	alDeleteBuffers (1, &buffer);
}

bool exaBuffer::loadfile (const char*file)
{
	int format, size;
	float freq;
	ALvoid* data = alutLoadMemoryFromFile (file, &format, &size, &freq);
	if (!data) {
		checkALError();
	} else {
		alBufferData (buffer, format, data, size, (int) freq);
		free (data);
	}
	return true;
}

bool exaBuffer::loadRAW (const char*file, int format, unsigned int freq,
                         int headersize)
{
	FILE*f = fopen (file, "rb");
	if (!f) return false;
	fseek (f, 0, SEEK_END);
	unsigned int len = ftell (f);
	char* buf = (char*) malloc (len);
	if (!buf) {
		fclose (f);
		return false;
	}
	fseek (f, 0, SEEK_SET);
	fread (buf, 8192, len / 8192, f);  //read as big blocks as we are able to
	fread (buf + len - (len % 8192), len % 8192, 1, f);
	fclose (f);
	alBufferData (buffer, format, buf, len, freq);
	return true;
}

bool exaBuffer::memoryload (const void* pmem, unsigned int size,
                            int format, unsigned int freq)
{
	alBufferData (buffer, format, pmem, size, freq);
	return true;
}

void exaBuffer::release()
{
	exaSound.adoptbuffer (this);
}

void exaBuffer::destroy()
{
	delete this;
}

//this should be fixed as soon as OpenAL devs enable
//alGetBufferState for public use
/*int exaBuffer::state()
{
	switch(_alGetBidState(buffer)){
		case AL_PENDING:
			return 2;
		case AL_PROCESSED:
			return 1;
		case AL_UNUSED:
			return 0;
	}
	return 0;
}*/

exaSource::exaSource()
{
	alGenSources (1, &source);
}

exaSample::exaSample (exaBuffer* b)
{
	if (b) setbuffer (b);  //possible NULL (default val)
}

exaSource::~exaSource()
{
	alDeleteSources (1, &source);
}

void exaSample::setbuffer (exaBuffer* b)
{
	alSourcei (source, AL_BUFFER, b->getalbuffer());
}

void exaSource::destroy()
{
	delete this;
}

void exaSample::release()
{
	exaSound.adoptsample (this);
}

int exaSource::isplaying()
{
	int state;
	alGetSourcei (source, AL_SOURCE_STATE, &state);
	switch (state) {
	case AL_INITIAL:
	case AL_STOPPED:
		return 0;
	case AL_PLAYING:
		return 1;
	case AL_PAUSED:
		return 2;
	}
	return 0;
}

void exaSource::cone (vector orientation, float innerangle, float outerangle,
                      float outervolume)
{
	alSourcefv (source, AL_ORIENTATION, orientation.v);
	alSourcef (source, AL_CONE_INNER_ANGLE, innerangle);
	alSourcef (source, AL_CONE_OUTER_ANGLE, outerangle);
	alSourcef (source, AL_CONE_OUTER_GAIN, outervolume);
}


bool exaStream::load (const char* fn, int t, int bufs, int bufn)
{
	int i;

	if (type) unload();


	exaOGGDecoder* od;
	if (t == 0) { //guess type from file extension
		if (strlen (fn) < 3) return false;
		if (strcasecmp (fn + (strlen (fn) - 3), "ogg") == 0) t = EXA_STREAM_OGG;
		else t = EXA_STREAM_NOT_LOADED;
	}

	switch (t) {

	case EXA_STREAM_OGG:

		decdata = malloc (sizeof (exaOGGDecoder));
		od = (exaOGGDecoder*) decdata;
		if (!od) return false;

		od->file = fopen (fn, "rb");
		if (!od->file) {
			free (od);
			return false;
		}

		if (ov_open (od->file, & (od->stream), NULL, 0)) {
			fclose (od->file);
			free (od);
			return false;
		}

		od->info = ov_info (& (od->stream), -1);
		od->comment = ov_comment (& (od->stream), -1);

		if (od->info->channels == 1) format = EXA_F_MONO16;
		else format = EXA_F_STEREO16;
		break;

	default:
		return false;
	}

	type = t;

	bufsize = bufs;
	bufcount = bufn;
	buffer = new exaBuffer[bufn];

	for (i = 0; i < bufcount; ++i) {
		fillbuffer (buffer[i].buffer);
		alSourceQueueBuffers (source, 1, & (buffer[i].buffer));
		nextbuf = 0;
	}
	return true;
}

void exaStream::update()
{
	if (!type) return;
	int procd, i;
	ALuint tbuf;

	alGetSourcei (source, AL_BUFFERS_PROCESSED, &procd);

	/*
	 * When queueing buffers accordingly to the numbers returned from
	 * unqueuebuffers(), queue disorders happen. It's better to count
	 * buffers yourself.
	 */

	for (i = 0; i < procd; ++i) {
		alSourceUnqueueBuffers (source, 1, &tbuf);
	}

	while (procd--) {
		if (fillbuffer (buffer[nextbuf].buffer)) {
			alSourceQueueBuffers (source, 1, & (buffer[nextbuf].buffer));
			if (!isplaying()) play();  //handle buffer underflows.
		}
		++nextbuf;
		if (nextbuf >= bufcount) nextbuf = 0;
	}
}

void exaStream::unload()
{
	if (!type) return;
	int queued;
	ALuint tbuf;
	stop();

	alGetSourcei (source, AL_BUFFERS_QUEUED, &queued);
	while (queued--)
		alSourceUnqueueBuffers (source, 1, &tbuf);


	switch (type) {
	case EXA_STREAM_OGG:
		ov_clear (& ( (exaOGGDecoder*) decdata)->stream);
		break;
	default:
		break;
	}

	delete [] buffer;

	free (decdata);
	decdata = NULL;
	type = EXA_STREAM_NOT_LOADED;
}

bool exaStream::fillbuffer (ALuint buf)
{
	char data[EXA_DECODER_BUFFER_SIZE];
	int size = 0;
	exaOGGDecoder* od;
	int result;

	switch (type) {
	case EXA_STREAM_OGG:
		od = (exaOGGDecoder*) decdata;
		int bitstream;

		while (size < bufsize) {
			result = ov_read (& (od->stream),
			                  data + size,
			                  bufsize - size,
			                  0, 2, 1,
			                  &bitstream);

			if (result > 0)
				size += result;
			else if (result < 0)
				return false;
			else
				break;
		}

		if (size == 0) return false;
		alBufferData (buf, format, data, size, od->info->rate);

		return true;

	default:
		return false;
	}
}


float exaStream::length()
{
	switch (type) {
	case EXA_STREAM_OGG:
		return (float) ov_time_total (& ( ( (exaOGGDecoder*) decdata)->stream), -1);
	default:
		return -1;
	}
}

float exaStream::tell()
{
	switch (type) {
	case EXA_STREAM_OGG:
		return (float) ov_time_tell (& ( ( (exaOGGDecoder*) decdata)->stream));
	default:
		return -1;
	}
}



