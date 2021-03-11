#ifndef _UTILS_H_
#define _UTILS_H_

#define SK_WINDOW_TITLE	"SK Tiny Engine"
#define SK_INTRO_SOURCES
//#define SK_DEMO_SOURCES

#define USEMEMLOADRESOURCE
#define USEFMOD TRUE

#ifdef __WIN32__
#include <windows.h>
#include <conio.h>
#include "resources/resource.h"
#include "minifmod/minifmod.h"
#endif /* __WIN32__ */
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
//#include "math3.h"
#include "AsmMath4.h"
#ifdef __LINUX__
#include "SDL.h"
#endif /* __LINUX__ */


#define RANDOM_FLOAT (((float)rand()-(float)rand())/(float)RAND_MAX)
#define CHECK_RANGE(x,min,max) ((x= (x<min  ? min : x<max ? x : max)))
#define DEG_TO_RAD(angle)  ((angle)*PIOVER180)
#define RAD_TO_DEG(radians) ((radians)*PIUNDER180)
#define SQUARE(number) (number*number)

struct UB_RGB_A {
	GLubyte	r;
	GLubyte	g;
	GLubyte	b;
	GLubyte	a;

	UB_RGB_A &operator = (UB_RGB_A const &v) {
		r = v.r;
		g = v.g;
		b = v.b;
		a = v.a;
		return(*this);
	}

	UB_RGB_A &operator + (UB_RGB_A const &v) {
		r += v.r;
		g += v.g;
		b += v.b;
		a += v.a;
		return(*this);
	}
};

struct RGB_A {
	float	r;
	float	g;
	float	b;
	float	a;

	RGB_A &operator = (RGB_A const &v) {
		r = v.r;
		g = v.g;
		b = v.b;
		a = v.a;
		return(*this);
	}

	RGB_A &operator + (RGB_A const &v) {
		r += v.r;
		g += v.g;
		b += v.b;
		a += v.a;
		return(*this);
	}
};

struct UV_COORD {
	float u;
	float v;
};


/* Timing vars */
static float      timer_time;
#ifdef __WIN32__
static __int64    timer_start;
static __int64    timer_frq;
#endif /* __WIN32__ */
#ifdef __LINUX__
static Uint32     timer_start;
static Uint32     timer_frq;
#endif /* __LINUX__ */
static float      timer_lastup;
static float      timer_fps;


/* Timing Functions */
inline void skInitTimer()
{
#ifdef __WIN32__
	QueryPerformanceCounter((LARGE_INTEGER *)&timer_start);
	QueryPerformanceFrequency((LARGE_INTEGER *)&timer_frq);
#endif /* __WIN32__ */
#ifdef __LINUX__
	timer_start = SDL_GetTicks();
#endif /* __LINUX__ */
	timer_lastup = 0;
}

inline float skGetTime()
{
#ifdef __WIN32__
    return timer_time;
#endif /* __WIN32__ */
#ifdef __LINUX__
    return static_cast<float>(SDL_GetTicks() - (double)timer_start)/1000.f;;
#endif /* __LINUX__ */
}

inline float skTimerFrame()
{
#ifdef __WIN32__
  __int64 a;
	QueryPerformanceCounter((LARGE_INTEGER *)&a);
	
	timer_time = (float)(a - timer_start)/(float)(timer_frq);
#endif /* __WIN32__ */
#ifdef __LINUX__
	timer_time = (float)(skGetTime() - timer_start)/(float)(timer_frq);
#endif /* __LINUX__ */

	/* do not change this pls */
	timer_fps = 1 / ((timer_time - timer_lastup)+ 0.0001f);
	timer_lastup = timer_time;


	return timer_time;
}

inline float skGetFPS()
{
	return (float)timer_fps;
}


/************************************************************/
/*            Other useless stuff                           */
/************************************************************/

void panViewOrtho();
void panViewPerspective();
void panViewPerspectiveFOV(float fov);

#endif
