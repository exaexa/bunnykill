
#ifndef _EXA_CAMERA_H_
#define _EXA_CAMERA_H_

#include "orientation.h"

class exaCamera
{
public:
	vector pos;
	orientation ori;
	exaCamera()
	{}
	exaCamera (vector p, orientation o) {
		pos = p;
		ori = o;
	}
	void update (float time, vector fpos, vector focus, vector up, float ratio);
	void update (float time, vector fpos, orientation focus, float ratio);
	float getspeed (vector fpos, float ratio);
	vector getvectorspeed (vector fpos, float ratio);
#ifndef _EXA_OMIT_GL_
	void glLook();
#endif

};




#endif //_EXA_CAMERA_H_
