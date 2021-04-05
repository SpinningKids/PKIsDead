
/************************************
  REVISION LOG ENTRY
  Revision By: Pan
  Revised on 2/2/2002 10:28:24 PM
  Comments: Creation
 ************************************/

#ifndef GLFONT_H_
#define GLFONT_H_

#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#include <gl/GL.h>
#else
#include <gl/GL.h>
#include "glf.h"
#endif

class GLFont {
    GLYPHMETRICSFLOAT gmf[96];
    GLuint base;
public:
#ifdef WIN32
    GLFont(HDC hDC, const char* s);
#else
    GLFont(GLF_MEMFILE* font);
#endif

    void printChar(char c, float lsx, float lsy, float lr, float tracking) const;

    float getWidth(char c, float tracking = 0) const { return gmf[c - 32].gmfCellIncX * (tracking + 1); }
    float getHeight(char c) const { return gmf[c - 32].gmfBlackBoxY; }
    void skipChar(char c, float tracking = 0) const { glTranslatef(getWidth(c, tracking), 0, 0); }

    ~GLFont();
};

#endif // GLFONT_H_
