
/************************************
  REVISION LOG ENTRY
  Revision By: pan
  Revised on 2/16/2002 10:11:37 AM
  Comments: creation
 ************************************/

#ifndef _GLTEXTURE_H_
#define _GLTEXTURE_H_

#pragma once

#ifdef WIN32
#include <windows.h>
#include <gl/gl.h>
#endif /* WIN32 */

#ifdef __LINUX__
#include <GL/gl.h>
#endif /* __LINUX__ */

class GLTexture {
  GLfloat *mem;
  int bpp, logsize, size;
  GLuint txt;
  bool init;
public:
  explicit GLTexture(int _logsize);

  ~GLTexture() {
    delete[] mem;
    glDeleteTextures(1,&txt);
  }

  GLfloat *getImage() { return mem; }
  int getSize() { return size; }

  void update();
  void use();
  int getID(){ return txt; }
};

#endif //_GLTEXTURE_H_
