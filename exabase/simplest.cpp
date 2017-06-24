
#include "exa-base.h"
#include <unistd.h>



void initgl()
{
	glShadeModel (GL_SMOOTH);
	glDisable (GL_DEPTH_TEST);
	glFrontFace (GL_CCW);
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//max hint
	glMatrixMode (GL_PROJECTION);
	//switch to projection matrix
	glLoadIdentity();
	//reset it
	gluPerspective (75, 4.0 / 3, 0.1, 100);
	//fov, 4/3 screen dimension, near clip, far clip
	glMatrixMode (GL_MODELVIEW);
	//back to the model

	glClearColor (0, 0, 0, 0);
	//background color - R,G,B,A

}

void render()
{
	static float rot = 0;
	int i;

	rot += 0.05;

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity(); //reset modelview
	glClear (GL_COLOR_BUFFER_BIT); // |depth buffer bit if
	//we used depths

	glEnable (GL_BLEND);
	glBlendFunc (GL_ONE, GL_ONE);
	//this is normal additive blending

	glColor3ub (64, 128, 192); //set a color

	glTranslatef (0, 0, -10); //move 10 units forward
	glRotatef (-30, 1, 0, 0); //tilt a little
	glRotatef (rot, 0, 1, 0); //rotate tube
	glRotatef (4 * rot, 0, 0, 1); //roll the tube fast
	for (i = 0; i < 360; i += 36) {
		glPushMatrix(); //save space position
		glRotatef (i, 0, 0, 1);
		glBegin (GL_LINES); //draw lines
		glVertex3f (2, 0, -5);
		glVertex3f (0, 2, 5);
		glEnd();  //do the drawing
		glPopMatrix(); //return the position to be actual
	}
	glDisable (GL_BLEND);
	glFinish(); //wait till drawing finishes
	exaglSwapBuffers(); //bring it to foreground
}

int main()
{
	bool done = false;
	exaSetParams (1024, 768, 32); //set your own
	if (!exaInit()) return -1;

	initgl();
	while (!done) {
		exaUSleep (20);
		exaUpdate();
		//update cycle - wm and KB stuff
		if (exaIsKeyDown (EKEY_ESCAPE)) done = true;
		//exit on escape
		if (exaIsKeyDown (EKEY_TAB)) exaIconifyWindow();
		//iconify=minimize
		if (!exaIsIconified()) render();
		//render only if it will be seen
	}
	exaShutdown();
	return 0;
}
