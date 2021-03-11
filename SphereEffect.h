// SphereEffect.h


#if !defined(SPHERE_EFFECT_H)

#include "AsmMath4.h"

int initSphereObject( int g_subd, float radius );
void dSphereEffect(rgb_a pcolor, rgb_a vcolor, bool base, bool lines );
void unloadSphereEffect( void );


#endif	// SPHERE_EFFECT_H
