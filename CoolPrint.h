

/************************************
  REVISION LOG ENTRY
  Revision By: Pan
  Revised on 2/7/2002 11:55:42 PM
  Comments: Creation
 ************************************/

#ifndef _COOLPRINT_H_
#define _COOLPRINT_H_

#ifdef _MSC_VER
#pragma once
#pragma pack(push, _PACK_COOLPRINT_H_)
#endif //_MSC_VER

#include "GLFont.h"

void CoolPrint1(GLFont const &font, int n, float t, float st, float on, float fd, float en, float x, float y, float gs, float ratio, float tracking, const char *fmt, ...);

#ifdef _MSC_VER
#pragma pack(pop, _PACK_COOLPRINT_H_)
#endif //_MSC_VER

#endif //_COOLPRINT_H_
