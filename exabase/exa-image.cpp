
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "exa-image.h"
#include <png.h>

#include <GL/glu.h>

#pragma pack(push,1)

typedef struct
{
	unsigned short id;
	unsigned int fileSize;
	unsigned short reserved[2];
	unsigned int headerSize;
	unsigned int infoSize;
	unsigned int width;
	unsigned int height;
	unsigned short biPlanes;
	unsigned short bits;
	unsigned int biCompression;
	unsigned int biSizeImage;
	unsigned int biXPelsPerMeter;
	unsigned int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
}
BMPHEADER;

#pragma pack(pop)


bool exaImageReadBMP (const char*fn, int*x, int*y, int*bytes, char**data)
{
	FILE*f = fopen (fn, "rb");
	if (!f) return false;

	BMPHEADER bh;

	if (!fread (&bh, sizeof (BMPHEADER), 1, f) )
		goto close_err;

	if (bh.id != * (uint16_t*) "BM")
		goto close_err;

	if ( (bh.headerSize != 54) || (bh.infoSize != 40) )
		goto close_err;

	*x = bh.width;
	*y = bh.height;
	*bytes = bh.bits / 8;
	*data = (char*) malloc ( (*x) * (*y) * (*bytes) );
	fread (*data, (*x) * (*y) * (*bytes), 1, f);
	fclose (f);
	return true;
close_err:
	fclose (f);
	return false;
}

bool exaImageWriteBMP (const char*fn, int x, int y, int bytes, char*data,
                       bool bgrswap)
{
	if ( (!data) || (!fn) ) return false;
	if ( (x < 0) || (y < 0) || (bytes != 3) ) return false;

	FILE*f = fopen (fn, "wb");
	if (!f) return false;

	BMPHEADER bh;

	memset ( (char*) &bh, 0, sizeof (BMPHEADER) );

	memcpy (& (bh.id), "BM", 2);
	bh.fileSize = sizeof (BMPHEADER) + x * y * 3;
	bh.reserved[0] = bh.reserved[1] = 0;
	bh.headerSize = 54;
	bh.infoSize = 40;
	bh.width = x;
	bh.height = y;
	bh.biSizeImage = 3 * x * y;
	bh.biPlanes = 1;
	bh.bits = 24;
	bh.biCompression = 0L;

	if (bgrswap) {
		char t;
		for (int i = 0;i < x*y;++i) {
			t = data[3*i];
			data[3*i] = data[3*i+2];
			data[3*i+2] = t;
		}
	}

	fwrite (&bh, sizeof (bh), 1, f);
	fwrite (data, 3*x*y, 1, f);
	fclose (f);

	return true;
}

bool exaImageReadPNG (const char*fn, int*x, int*y, int*bytes, char**data)
{
	char header[8];

	FILE *fp = fopen (fn, "rb");
	if (!fp)
		return false;

	fread (header, 8, 1, fp);
	if (png_sig_cmp ( (png_byte*) header, 0, 8) ) {
		fclose (fp);
		return false;
	}

	fseek (fp, 0, SEEK_SET);

	png_structp png_ptr = png_create_read_struct
	                      (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		fclose (fp);
		return false;
	}

	png_infop info_ptr = png_create_info_struct (png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct (&png_ptr,
		                         (png_infopp) NULL, (png_infopp) NULL);
		fclose (fp);
		return false;
	}

	png_init_io (png_ptr, fp);
	png_read_png (png_ptr, info_ptr,
	              PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING, NULL);

	int h, w, b;

	h = info_ptr->height;
	w = info_ptr->width;

	switch (info_ptr->color_type) {
	case PNG_COLOR_TYPE_GRAY:
		b = 1;
		break;
	case PNG_COLOR_TYPE_GA:
		b = 2;
		break;
	case PNG_COLOR_TYPE_RGB:
		b = 3;
		break;
	case PNG_COLOR_TYPE_RGBA:
		b = 4;
		break;
	default:
		png_destroy_read_struct (&png_ptr, &info_ptr, NULL);

		fclose (fp);
		return false;
	}

	png_bytep *rows;
	rows = png_get_rows (png_ptr, info_ptr);
	int i;
	char*dat = (char*) malloc (w * h * b);
	for (i = 0;i < h;++i)
		memcpy (dat + i*w*b, rows[h-i-1], w*b);

	png_destroy_read_struct (&png_ptr, &info_ptr, NULL);

	fclose (fp);

	*x = w;
	*y = h;
	*bytes = b;
	*data = dat;

	return true;
}

bool exaImageWritePNG (const char*fn, int x, int y, int bytes, char*data)
{
	if ( (x < 0) || (y < 0) || (bytes < 1) || (bytes > 4) ) return false;
	if ( (!data) || (!fn) ) return false;
	FILE*f = fopen (fn, "wb");
	if (!f) return false;

	png_structp png_ptr = png_create_write_struct
	                      (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		fclose (f);
		return false;
	}

	png_infop info_ptr = png_create_info_struct (png_ptr);
	if (!info_ptr) {
		png_destroy_write_struct (&png_ptr,
		                          (png_infopp) NULL);
		fclose (f);
		return false;
	}

	png_init_io (png_ptr, f);

	png_set_compression_level (png_ptr, Z_BEST_COMPRESSION);

	int colortype;

	switch (bytes) {
	case 1:
		colortype = PNG_COLOR_TYPE_GRAY;
		break;
	case 2:
		colortype = PNG_COLOR_TYPE_GA;
		break;
	case 3:
		colortype = PNG_COLOR_TYPE_RGB;
		break;
	case 4:
		colortype = PNG_COLOR_TYPE_RGBA;
		break;
	default:
		png_destroy_write_struct (&png_ptr, &info_ptr);
		fclose (f);
		return false;
	}

	png_set_IHDR (png_ptr, info_ptr, x, y, 8, colortype,
	              PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
	              PNG_FILTER_TYPE_DEFAULT);

	png_bytep* rows = (png_bytep*) malloc (y * sizeof (png_bytep) );

	int i;
	for (i = 0;i < y;++i) rows[y-i-1] = (png_bytep) (data + (x * bytes * i) );
	png_set_rows (png_ptr, info_ptr, rows);

	png_write_png (png_ptr, info_ptr, 0, NULL);

	png_destroy_write_struct (&png_ptr, &info_ptr);

	free (rows);

	fclose (f);

	return true;
}

static bool _ispw2 (int a)
{
	while (a) {
		if (a&1) {
			a >>= 1;
			if (a) return false;
		} else a >>= 1;
	}
	return true;
}

GLuint exaImageMakeGLTex (int x, int y, int b, char* d,
                          bool fil, bool clmp, bool mpmp)
{
	GLuint tex;
	if (!d) return 0;
	if ( (x < 0) || (y < 0) || (b < 1) || (b > 4) ) return 0;
	if (!_ispw2 (x) ) return 0;
	if (!_ispw2 (y) ) return 0;

	int glmode = 0;
	switch (b) {
	case 1:
		glmode = GL_LUMINANCE;
		break;
	case 2:
		glmode = GL_LUMINANCE_ALPHA;
		break;
	case 3:
		glmode = GL_RGB;
		break;
	case 4:
		glmode = GL_RGBA;
		break;
	}
	glGenTextures (1, &tex);
	glBindTexture (GL_TEXTURE_2D, tex);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
	                 clmp ? GL_CLAMP : GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
	                 clmp ? GL_CLAMP : GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
	                 fil ? GL_LINEAR : GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	                 mpmp ? (fil ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_NEAREST)
			                 : (fil ? GL_LINEAR : GL_NEAREST) );
	if (mpmp) gluBuild2DMipmaps (GL_TEXTURE_2D, b, x, y,
		                             glmode, GL_UNSIGNED_BYTE, d);
	else glTexImage2D (GL_TEXTURE_2D, 0, b, x, y,
		                   0, glmode, GL_UNSIGNED_BYTE, d);
	return tex;
}

void exaImageDelGLTex (GLuint t)
{
	if (glIsTexture (t) ) glDeleteTextures (1, &t);
}

void exaImageFree (char*data)
{
	if (data) free (data);
}

