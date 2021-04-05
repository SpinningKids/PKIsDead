// SphereEffect.h


#ifndef SPHERE_EFFECT_H_
#define SPHERE_EFFECT_H_

#include "AsmMath4.h"

int initSphereObject(int g_subd, float radius);
void dSphereEffect(rgb_a pcolor, rgb_a vcolor, bool base);
void unloadSphereEffect();

#endif	// SPHERE_EFFECT_H_
