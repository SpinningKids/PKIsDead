
/************************************
  REVISION LOG ENTRY
  Revision By: Pan
  Revised on 2/2/2002 10:28:24 PM
  Comments: Creation
 ************************************/

#ifndef _GLFONT_H_
#define _GLFONT_H_

#pragma once

#ifdef WIN32
#include <windows.h>
#include <gl/gl.h>
#endif /* WIN32 */

#ifdef __LINUX__
#include <GL/gl.h>
#include "glf.h"
#endif /* __LINUX__ */

class GLFont {
#ifdef WIN32
  GLYPHMETRICSFLOAT gmf[256];
#endif /* WIN32 */
#ifdef __LINUX__
  GLYPHMETRICSFLOAT gmf[96];
#endif /* __LINUX__ */
  GLuint base;
public:
#ifdef WIN32
  GLFont(HDC hDC, const char *s);
#endif /* WIN32 */
#ifdef __LINUX__
  GLFont(GLF_MEMFILE *font);
#endif /* __LINUX__ */

  void printf(const char *fmt, ...) const;
  void printfx(float x, float y, float gs, float gr, float lsx, float lsy, float lr, float tracking, const char *fmt, ...) const;

  void printChar(char c, float lsx, float lsy, float lr, float tracking) const;

  float getWidth(char c, float tracking = 0) const { return gmf[c-32].gmfCellIncX*(tracking+1); }
  float getHeight(char c) const { return gmf[c-32].gmfBlackBoxY; }
  void skipChar(char c, float tracking = 0) const { glTranslatef(getWidth(c, tracking), 0, 0); }

  ~GLFont();
};

#endif //_GLFONT_H_
