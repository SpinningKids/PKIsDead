
/************************************
  REVISION LOG ENTRY
  Revision By: Pan
  Revised on 2/2/2002 10:28:24 PM
  Comments: Creation
 ************************************/

#ifndef GLFONT_H_
#define GLFONT_H_

#pragma once

#ifdef WIN32
#include <Windows.h>
#include <GL/gl.h>
#else
#include <GL/gl.h>
#include "glf.h"
#endif /* __LINUX__ */

class GLFont {
  GLYPHMETRICSFLOAT gmf[96];
  GLuint base;
public:
#ifdef WIN32
  GLFont(HDC hDC, const char *s);
#else
  GLFont(GLF_MEMFILE *font);
#endif

  void printf(const char *fmt, ...) const;
  void printfx(float x, float y, float gs, float gr, float lsx, float lsy, float lr, float tracking, const char *fmt, ...) const;

  void printChar(char c, float lsx, float lsy, float lr, float tracking) const;

  float getWidth(char c, float tracking = 0) const { return gmf[c-32].gmfCellIncX*(tracking+1); }
  float getHeight(char c) const { return gmf[c-32].gmfBlackBoxY; }
  void skipChar(char c, float tracking = 0) const { glTranslatef(getWidth(c, tracking), 0, 0); }

  ~GLFont();
};

#endif // GLFONT_H_
