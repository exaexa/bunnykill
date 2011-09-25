
#ifndef _EXA_SOUND_H_
#define _EXA_SOUND_H_


#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include "vector.h"
#include <set>

#define EXA_F_MONO8 AL_FORMAT_MONO8
#define EXA_F_MONO16 AL_FORMAT_MONO16
#define EXA_F_STEREO8 AL_FORMAT_STEREO8
#define EXA_F_STEREO16 AL_FORMAT_STEREO16
#define EXA_DM_NONE AL_NONE
#define EXA_DM_LINEAR AL_LINEAR_DISTANCE_CLAMPED
#define EXA_DM_INVERSE AL_INVERSE_DISTANCE_CLAMPED
#define EXA_DM_EXPONENT AL_EXPONENT_DISTANCE_CLAMPED


#ifndef exastorage
#define exastorage std::vector
#endif

using std::set;

class exaBuffer;
class exaSample;
class exaStream;
class exaSoundSystem;

class exaSoundSystem
{  //designed to be a global variable!
private:
	set<exaSample*> samples;
	set<exaBuffer*> buffers;
	set<exaStream*> streams;
	bool isinit;
public:
	exaSoundSystem()
{};
	~exaSoundSystem()
	{};

	bool init();
	void shutdown();

	exaBuffer* createbuffer();
	exaSample* createsample (exaBuffer* b = 0);
	exaStream* createstream();

	void adoptsample (exaSample* s);
	void adoptbuffer (exaBuffer* b);
	//we don't want adopting streams.
	void stopadopted();
	void updateadopted();

	inline void globalvolume (float vol)
	{
		alListenerf (AL_GAIN, vol);
	}
	inline void listenerpos (vector pos)
	{
		alListenerfv (AL_POSITION, pos.v);
	}
	inline void listenerspd (vector spd)
	{
		alListenerfv (AL_VELOCITY, spd.v);
	}
	void listenerori (vector forward, vector top);

	inline void soundspeed (float s)
	{
		alDopplerVelocity (s);
	}
	inline void dopplerfactor (float df)
	{
		alDopplerFactor (df);
	}
	//TRICKY, if doppler refuses to work,, change alDopplerVelocity
	//to alSpeedOfSound. This is a compatibility problem. Solve.

	inline void distancemodel (int m)
	{
		alDistanceModel (m);
	}
};


#ifdef _EXA_SOUND_CPP_
exaSoundSystem exaSound;     /// <<<----  USE THIS globally
#else
extern exaSoundSystem exaSound;
#endif



class exaBuffer
{
	ALuint buffer;
public:
	friend void exaSoundSystem::updateadopted();
	friend class exaStream;

	exaBuffer();
	~exaBuffer();
	inline ALuint getalbuffer()
	{
		return buffer;
	}
	bool loadfile (const char* filename);
	bool loadRAW (const char* filename, int format, unsigned int freq, int rawheadersize = 0);
	bool memoryload (const void* pmem, unsigned int size, int format, unsigned int freq);

	//disabled till implemented by OpenAL devs
	//int state(); 	//uses internal al function to hack it.
	//0 means NOT playing, 1=buf. is being processed; 2=waiting in queue
	void release(); //safe, use this
	void destroy(); //unsafe, strict one deletes without questions.
	//May produce page fault, if misused.

};

class exaSource
{
protected:
	ALuint source;
public:
	exaSource();
	~exaSource();
	void destroy();
	int isplaying(); //0/1, 2=paused
	inline void play()
	{
		alSourcePlay (source);
	}
	inline void pause()
	{
		alSourcePause (source);
	}
	inline void stop()
	{
		alSourceStop (source);
	}
	inline void rewind()
	{
		alSourceRewind (source);
	}
	//start from origin again

	inline float elapsedseconds()  //from start, if looping, jumps to 0
	{float s;
		alGetSourcef (source, AL_SEC_OFFSET, &s);
		return s;
	}

	inline void relative (bool r) //relative from listener, default in absolute space
{alSourcei (source, AL_SOURCE_RELATIVE, r ? AL_TRUE : AL_FALSE);
	}

	inline void position (vector p) //set position in space
	{alSourcefv (source, AL_POSITION, p.v);
	}

	inline void speed (vector s) //speed in space
	{alSourcefv (source, AL_VELOCITY, s.v);
	}

	inline void looping (bool l) //looping true/false
{alSourcei (source, AL_LOOPING, l ? AL_TRUE : AL_FALSE);
	}

	inline void volume (float vol) //gain, 1 is normal, 0 silence. >1 isn't always functional
	{alSourcef (source, AL_GAIN, vol);
	}

	inline void pitch (float p) //1=normal speed, 2= +12 halftones, 0.5=-12 halftones
	//HAS A BOORING LIMIT FROM OPENAL DEVS - [0.5 - 2] -- fix avail?
	{alSourcef (source, AL_PITCH, p);
	}

	void cone (vector orientation, float innerangle = 0, float outerangle = 0,
	           float outervolume = 1);  //set CONE s<
	void uncone();  //unset cone directions
	inline void referencedist (float d) //reference distance
	{alSourcef (source, AL_REFERENCE_DISTANCE, d);
	}
	inline void maxdist (float d) //maximal distance
	{alSourcef (source, AL_MAX_DISTANCE, d);
	}
	inline void rolloff (float r) //rolloff factor
	{alSourcef (source, AL_ROLLOFF_FACTOR, r);
	}

};

class exaSample : public exaSource
{
	ALuint buffer;
	using exaSource::source;
public:
	friend void exaSoundSystem::adoptsample (exaSample*);

	exaSample();
	exaSample (exaBuffer* b);
	~exaSample()
	{
		destroy();
	}
	void setbuffer (exaBuffer* b);
	void release(); //lets itself playing and when stops, is properly deleted.
	//see exaSound.adoptsource()
};

#ifndef _EXA_OMIT_STREAMING_


#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>


#define EXA_DECODER_BUFFER_SIZE (65536)
#define EXA_DECODER_BUFFER_COUNT (80)

#define EXA_STREAM_NOT_LOADED 0
#define EXA_STREAM_OGG 1

typedef struct
{
	FILE*file;
	OggVorbis_File stream;
	vorbis_info* info;
	vorbis_comment* comment;
}
exaOGGDecoder;

class exaStream : public exaSource
{
	exaBuffer *buffer;
	int bufsize;
	int bufcount;
	int nextbuf;
	int type;
	void* decdata;
	int format;
	bool fillbuffer (ALuint buffer);
public:
	exaStream()
	{
		type = EXA_STREAM_NOT_LOADED;
	}
	~exaStream()
	{
		if (type) unload();
	}

	bool load (const char*filename, int t = EXA_STREAM_NOT_LOADED, int buffersize = 65536, int buffercount = 8);
	//TODO, create something like loadstream(FILE*,type)

	void unload();
	void update();
	float length();
	float tell();
};

#endif // _EXA_OMIT_STREAMING_


#endif
