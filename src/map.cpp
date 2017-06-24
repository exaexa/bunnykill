
#include "game.h"
#include "map.h"
#include "exa-image.h"
#include <stdlib.h>
#include "common.h"


bool bkMap::init()
{
	int x, y, b;
	char*d;
	grass = grasschunk = mud = mudborder = mudchunk = 0;

	if (!exaImageReadPNG ("data/tex/grass.png", &x, &y, &b, &d))
		goto err_exit;
	grass = exaImageMakeGLTex (x, y, b, d);
	exaImageFree (d);
	if (!grass) goto err_exit;

	if (!exaImageReadPNG ("data/tex/grasschunk.png", &x, &y, &b, &d))
		goto err_exit;
	grasschunk = exaImageMakeGLTex (x, y, b, d);
	exaImageFree (d);
	if (!grasschunk) goto err_exit;

	if (!exaImageReadPNG ("data/tex/mud.png", &x, &y, &b, &d))
		goto err_exit;
	mud = exaImageMakeGLTex (x, y, b, d);
	exaImageFree (d);
	if (!mud) goto err_exit;

	if (!exaImageReadPNG ("data/tex/mudborder.png", &x, &y, &b, &d))
		goto err_exit;
	mudborder = exaImageMakeGLTex (x, y, b, d);
	exaImageFree (d);
	if (!mudborder) goto err_exit;

	if (!exaImageReadPNG ("data/tex/mudchunk.png", &x, &y, &b, &d))
		goto err_exit;
	mudchunk = exaImageMakeGLTex (x, y, b, d);
	exaImageFree (d);
	if (!mudchunk) goto err_exit;

	return true;
err_exit:
	release();
	return false;
}

void bkMap::release()
{
	exaImageDelGLTex (grass);
	exaImageDelGLTex (grasschunk);
	exaImageDelGLTex (mud);
	exaImageDelGLTex (mudchunk);
	exaImageDelGLTex (mudborder);
}

void bkMap::reset()
{
	floor = deque<complex>();
	platform = storage<bkPlatform>();
	platform_timeout = FRAND;
	update (0);
}

void bkMap::set (float position, float difficulty)
{
	float posdiff = position - pos;

	diff = difficulty;
	pos = position;
	storage<bkPlatform>::iterator pi;
	deque<complex>::iterator ci;

	platform_timeout -= posdiff;

	for (pi = platform.begin(); pi < platform.end(); ++pi) pi->pos.x -= posdiff;
	for (ci = floor.begin(); ci < floor.end(); ++ci) ci->x -= posdiff;
	game.sky.pos += 0.7 * posdiff;
	compute_new_map();
}

void bkMap::draw()
{
	glPushMatrix();
	glEnable (GL_TEXTURE_2D);
	glColor3ub (255, 255, 255);
	glTranslatef (pos, 0, 0);

	deque<complex>::iterator fi;
	for (fi = floor.begin(); fi + 1 < floor.end(); ++fi)
		draw_map_rect (complex (fi->x, 0),
		               complex ( ( (fi + 1)->x) - (fi->x), fi->y), false);

	storage<bkPlatform>::iterator i;
	for (i = platform.begin(); i < platform.end(); ++i)
		draw_map_rect (i->pos, i->size);

	//draw_map_rect(complex(0.2,0.4),complex(0.4,0.02));

	glPopMatrix();
}

void bkMap::update (float position)
{
	set (position, diff);
}

#define bun_w 0.0272

bool bkMap::solvecol (bkBunny& bunny)
{
	return solvecol (& (bunny.pos), & (bunny.spd), 0, bun_w, 0.5 * bun_w, 0.5 * bun_w,
	                 0.0, 0.0, 0.02, & (bunny.is_on_ground));
}

bool bkMap::solvecol (bkMeatPiece& piece)
{
	return solvecol (& (piece.pos), & (piece.spd),
	                 0.005, 0.005, 0.005, 0.005, 0.6, 0.1, 0, NULL);
}

bool bkMap::solvecol (complex* position, complex* spd,
                      float bbot, float btop, float blft, float brgt,
                      float refl, float minrefl, float topalign,
                      bool*is_on_ground)
{
	complex placeholder = complex (0, 0);
	if (!position) return false;
	if (!spd) spd = &placeholder;
	//the logic of deciding whether to measure and change the speed
	//is a little bit too complicated, so we made a placeholder

	deque<complex>::iterator ci;
	storage<bkPlatform>::iterator pi;

	float t = position->y + btop, b = position->y - bbot;
	float l = position->x - brgt, r = position->x + blft;
	l -= pos;
	r -= pos;
	if (is_on_ground) *is_on_ground = b == 0;

	bool collided = false;

	float db, dt, dl, dr;

	for (ci = floor.begin(); ci < floor.end() - 1; ++ci) {
		dl = ci->x - r;
		dr = l - (ci + 1)->x;
		dt = b - ci->y;
		if ( (dr <= 0) && (dl <= 0) && (dt <= 0)) {
			collided = true;
			if ( (dt > -topalign) || ( (dt > dl) && (dt > dr))) {   //top collision

				if (spd->y <= 0) {
					position->y = ci->y + bbot;
					if (fabs (spd->y) > minrefl)
						spd->y *= -refl;
					else spd->y = 0;
					if (is_on_ground) *is_on_ground = true;
				}

			} else if (dl > dr) { //left collision
				if (spd->x >= 0) {
					spd->x *= -refl;
					position->x = pos + ci->x - brgt;
				}
			} else { //right collision
				if (spd->x <= 0) {
					spd->x *= -refl;
					position->x = pos + (ci + 1)->x + blft;
				}
			}
		}
	}

	for (pi = platform.begin(); pi < platform.end(); ++pi) {
		dl = pi->pos.x - r;
		dr = l - (pi->pos.x + pi->size.x);
		dt = b - (pi->pos.y + pi->size.y);
		db = pi->pos.y - t;
		if ( (dl <= 0) && (dr <= 0) && (dt <= 0) && (db <= 0)) {
			collided = true;
			if ( (dt > -topalign) || ( (dt > db) && (dt > dr) && (dt > dl))) {   //top
				if (spd->y <= 0) {
					position->y = pi->pos.y + pi->size.y + bbot;
					if (fabs (spd->y) > minrefl)
						spd->y *= -refl;
					else spd->y = 0;
					if (is_on_ground) *is_on_ground = true;
				}
			} else if ( (db > dl) && (db > dr)) {  //bottom
				if (spd->y >= 0) {
					spd->y *= -refl;
					position->y = pi->pos.y - btop;
				}
			} else if (dl > dr) { //left
				if (spd->x >= 0) {
					spd->x *= -refl;
					position->x = pos + pi->pos.x - brgt;

				}
			} else { //right
				if (spd->x <= 0) {
					spd->x *= -refl;
					position->x = pos + pi->pos.x +
					              pi->size.x + blft;
				}
			}
		}

	}
	return collided;
}

void bkMap::compute_new_map()
{
	deque<complex>::iterator i;
	for (i = floor.begin(); i < floor.end(); ++i) if (i->x > 0) break;
	--i;
	--i;
	if (i >= floor.begin()) floor.erase (floor.begin(), i);
	if (floor.size() <= 0) floor.push_back (complex (-0.3 * FRAND, 0.1 + 0.3 * FRAND));
	while ( (floor.end() - 1)->x < 1) floor.push_back (
		    complex ( (floor.end() - 1)->x + 0.1 + 0.4 * FRAND,
		              (floor.end() - 1)->y + DFRAND / 10));

	for (i = floor.begin(); i < floor.end(); ++i) {
		if (i->y < 0.1) i->y = 0.1;
		if (i->y > 0.3) i->y = 0.3;
	}

	storage<bkPlatform>::iterator pi;
	for (pi = platform.begin(); pi < platform.end(); ++pi)
		if (pi->pos.x + pi->size.x < -0.01)
			platform.erase (pi--);

	if (platform_timeout < 0) {
		platform_timeout = 0.3 + FRAND * 4;
		bkPlatform bp;
		bp.pos = complex ( (floor.end() - 1)->x + 0.05 * FRAND,
		                   (floor.end() - 1)->y + 0.01 + 0.1 * FRAND);
		bp.size = complex (0.02 + 0.3 * FRAND, 0.01 + 0.03 * FRAND);
		platform.push_back (bp);
	}
}

#define grass_w 0.01
#define mud_w 0.005

void bkMap::draw_map_rect (complex start, complex size, bool draw_bottom)
{
	glPushMatrix();
	complex s = 5 * size;
	glTranslatef (start.x, start.y, 0);
	glBindTexture (GL_TEXTURE_2D, mud);
	glBegin (GL_QUADS);
	glTexCoord2f (0, 0);
	glVertex2f (0, 0);
	glTexCoord2f (s.x, 0);
	glVertex2f (size.x, 0);
	glTexCoord2f (s.x, s.y);
	glVertex2f (size.x, size.y);
	glTexCoord2f (0, s.y);
	glVertex2f (0, size.y);
	glEnd();

	//mud borders
	s *= 4;
	glBindTexture (GL_TEXTURE_2D, mudborder);
	glBegin (GL_QUADS);
	if (draw_bottom) {
		glTexCoord2f (0, 0);
		glVertex2f (0, -mud_w);
		glTexCoord2f (s.x, 0);
		glVertex2f (size.x, -mud_w);
		glTexCoord2f (s.x, 1);
		glVertex2f (size.x, + mud_w);
		glTexCoord2f (0, 1);
		glVertex2f (0, + mud_w);
	}

	//left one
	glTexCoord2f (0, 0);
	glVertex2f (mud_w, 0);
	glTexCoord2f (s.y, 0);
	glVertex2f (mud_w, size.y);
	glTexCoord2f (s.y, 1);
	glVertex2f (-mud_w, size.y);
	glTexCoord2f (0, 1);
	glVertex2f (-mud_w, 0);

	//right
	glTexCoord2f (0, 0);
	glVertex2f (size.x + mud_w, 0);
	glTexCoord2f (s.y, 0);
	glVertex2f (size.x + mud_w, size.y);
	glTexCoord2f (s.y, 1);
	glVertex2f (size.x - mud_w, size.y);
	glTexCoord2f (0, 1);
	glVertex2f (size.x - mud_w, 0);
	glEnd();
	if (draw_bottom) {
		glBindTexture (GL_TEXTURE_2D, mudchunk);
		glBegin (GL_QUADS);
		glTexCoord2f (0, 0);
		glVertex2f (-mud_w, -mud_w);
		glTexCoord2f (1, 0);
		glVertex2f (mud_w, -mud_w);
		glTexCoord2f (1, 1);
		glVertex2f (mud_w, mud_w);
		glTexCoord2f (0, 1);
		glVertex2f (-mud_w, mud_w);

		glTexCoord2f (0, 0);
		glVertex2f (size.x - mud_w, -mud_w);
		glTexCoord2f (1, 0);
		glVertex2f (size.x + mud_w, -mud_w);
		glTexCoord2f (1, 1);
		glVertex2f (size.x + mud_w, mud_w);
		glTexCoord2f (0, 1);
		glVertex2f (size.x - mud_w, mud_w);
		glEnd();
	}

	//grass
	glBindTexture (GL_TEXTURE_2D, grass);
	s /= 2;
	glBegin (GL_QUADS);
	glTexCoord2f (0, 0);
	glVertex2f (0, size.y - grass_w);
	glTexCoord2f (s.x, 0);
	glVertex2f (size.x, size.y - grass_w);
	glTexCoord2f (s.x, 1);
	glVertex2f (size.x, size.y + grass_w);
	glTexCoord2f (0, 1);
	glVertex2f (0, size.y + grass_w);
	glEnd();
	glBindTexture (GL_TEXTURE_2D, grasschunk);
	glBegin (GL_QUADS);
	glTexCoord2f (0, 0);
	glVertex2f (-grass_w, size.y - grass_w);
	glTexCoord2f (1, 0);
	glVertex2f (grass_w, size.y - grass_w);
	glTexCoord2f (1, 1);
	glVertex2f (grass_w, size.y + grass_w);
	glTexCoord2f (0, 1);
	glVertex2f (-grass_w, size.y + grass_w);

	glTexCoord2f (0, 0);
	glVertex2f (size.x - grass_w, size.y - grass_w);
	glTexCoord2f (1, 0);
	glVertex2f (size.x + grass_w, size.y - grass_w);
	glTexCoord2f (1, 1);
	glVertex2f (size.x + grass_w, size.y + grass_w);
	glTexCoord2f (0, 1);
	glVertex2f (size.x - grass_w, size.y + grass_w);
	glEnd();
	glPopMatrix();
}

