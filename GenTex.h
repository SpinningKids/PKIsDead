
#ifndef _GENTEX_H_
#define _GENTEX_H_

#pragma once

#include "GLTexture.h"

GLTexture *perlin(int logsize, float freq, float amp, float base, float k, bool wrap);
GLTexture *spot(float psize, float strenght);

#endif //_GENTEX_H_
