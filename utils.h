#ifndef UTILS_H_
#define UTILS_H_

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
