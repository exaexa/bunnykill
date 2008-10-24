
#ifndef _EXA_IMAGE_H_
#define _EXA_IMAGE_H_

#include <GL/gl.h>

bool exaImageReadBMP (const char*fn, int*x, int*y, int*bytes, char**data);
bool exaImageWriteBMP (const char*fn, int x, int y, int bytes, char*data,
                       bool isRGB);

bool exaImageReadPNG (const char*fn, int*x, int*y, int*bytes, char**data);
bool exaImageWritePNG (const char*fn, int x, int y, int bytes, char*data);

GLuint exaImageMakeGLTex (int x, int y, int bytes, char*data,
                          bool filter = true, bool clamp = false, bool mipmap = false);

void exaImageDelGLTex (GLuint tex);

void exaImageFree (char*data);

#endif

