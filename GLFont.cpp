
/************************************
  REVISION LOG ENTRY
  Revision By: Pan
  Revised on 2/2/2002 10:27:58 PM
  Comments: Class creation - ripped from rio's code
 ************************************/

#include "GLFont.h"

#ifdef WIN32
GLFont::GLFont(HDC hDC, const char* s) {
    HFONT	font = CreateFont(12,							// Height Of Font
        0,								// Width Of Font
        0,								// Angle Of Escapement
        0,								// Orientation Angle
        900,					// Font Weight
        FALSE,							// Italic
        FALSE,							// Underline
        FALSE,							// Strikeout
        ANSI_CHARSET,					// Character Set Identifier
        OUT_TT_PRECIS,					// Output Precision
        CLIP_DEFAULT_PRECIS,			// Clipping Precision
        ANTIALIASED_QUALITY,			// Output Quality
        FF_DONTCARE | DEFAULT_PITCH,		// Family And Pitch
        s);				// Font Name

    SelectObject(hDC, font);

    base = glGenLists(96);

    wglUseFontOutlines(hDC, 32, 96, base,
        0.015f,							// Deviation From The True Outlines
        0.f,							// Font Thickness In The Z Direction
        WGL_FONT_POLYGONS, gmf);

    DeleteObject(font);
}
#else
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glx.h>
#include "glf.h"

GLFont::GLFont(GLF_MEMFILE* font) {
    if (glfLoadFont((char*)font) != GLF_OK) {
        fprintf(stderr, "Error: cannot load font\n");
        exit(1);
    };
    base = glfGenLists(gmf);
}
#endif

void GLFont::printChar(char c, float lsx, float lsy, float lr, float tracking) const {
    c -= 32;

    glPushMatrix();

    glTranslatef(gmf[c].gmfCellIncX / 2.f, gmf[c].gmfBlackBoxY / 2.f, 0);
    glRotatef(lr, 0, 0, 1);
    glScalef(lsx, lsy, 1);
    glTranslatef(-gmf[c].gmfCellIncX / 2.f, -gmf[c].gmfBlackBoxY / 2.f, 0);

    glCallList(base + c);
    glPopMatrix();										// Pops The Display List Bits
    glTranslatef(gmf[c].gmfCellIncX * (tracking + 1), 0, 0);
}


GLFont::~GLFont() {
    glDeleteLists(base, 96);
}
