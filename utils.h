#ifndef UTILS_H_
#define UTILS_H_

#include "AsmMath4.h"

#define RANDOM_FLOAT (((float)rand()-(float)rand())/(float)RAND_MAX)
#define DEG_TO_RAD(angle)  ((angle)*PIOVER180)

/* Timing Functions */
void skInitTimer();
float skGetTime();
float skTimerFrame();
float skGetFPS();

/************************************************************/
/*            Other useless stuff                           */
/************************************************************/

void panViewOrtho();
void panViewPerspective();
void panViewPerspectiveFOV(float fov);

#endif // UTILS_H_
