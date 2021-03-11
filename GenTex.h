
#ifndef _GENTEX_H_
#define _GENTEX_H_

#ifdef _MSC_VER
#pragma once
#pragma pack(push, _PACK_GENTEX_H_)
#endif //_MSC_VER

#include "GLTexture.h"

GLTexture *perlin(int logsize, float freq, float amp, float base, float k, bool wrap);
GLTexture *spot(float psize, float strenght);

#ifdef _MSC_VER
#pragma pack(pop, _PACK_GENTEX_H_)
#endif //_MSC_VER

#endif //_GENTEX_H_
