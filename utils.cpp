
#include "utils.h"
#include "Globals.h"

void panViewOrtho() {
	glViewport(0,0,WIDTH,HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void panViewOrthoWidthHeight(int pwidth, int pheight) 
{
	glViewport(0,0,pwidth,pheight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, pwidth, pheight, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void panViewPerspective() {
	glViewport(0,0,WIDTH,HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,((float)WIDTH)/HEIGHT,0.1f,600.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void panViewPerspectiveFOV(float fov) 
{
	glViewport(0,0,WIDTH,HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov,((float)WIDTH)/HEIGHT,0.1f,600.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
