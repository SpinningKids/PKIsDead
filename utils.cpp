
#include "utils.h"

#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#else
#include "SDL.h"
#endif
#include <gl/GL.h>
#include <gl/GLU.h>
#include "AsmMath4.h"
#include "Globals.h"

void panViewOrtho() {
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void panViewOrthoWidthHeight(int pwidth, int pheight) {
    glViewport(0, 0, pwidth, pheight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, pwidth, pheight, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void panViewPerspective() {
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, ((float)WIDTH) / HEIGHT, 0.1f, 600.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void panViewPerspectiveFOV(float fov) {
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, ((float)WIDTH) / HEIGHT, 0.1f, 600.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/* Timing vars */
static float      timer_time;
#ifdef WIN32
static __int64    timer_start;
static __int64    timer_frq;
#else
static Uint32     timer_start;
static Uint32     timer_frq;
#endif
static float      timer_lastup;
static float      timer_fps;

/* Timing Functions */
void skInitTimer() {
#ifdef WIN32
    QueryPerformanceCounter((LARGE_INTEGER*)&timer_start);
    QueryPerformanceFrequency((LARGE_INTEGER*)&timer_frq);
#else
    timer_start = SDL_GetTicks();
#endif
    timer_lastup = 0;
}

float skGetTime() {
#ifdef WIN32
    return timer_time;
#else
    return static_cast<float>(SDL_GetTicks() - (double)timer_start) / 1000.f;;
#endif
}

float skTimerFrame() {
#ifdef WIN32
    __int64 a;
    QueryPerformanceCounter((LARGE_INTEGER*)&a);

    timer_time = (float)(a - timer_start) / (float)(timer_frq);
#else
    // TODO: Check - something is fishy here: skGetTime() already subtracts timer_start
    timer_time = (float)(skGetTime() - timer_start) / (float)(timer_frq);
#endif

    /* do not change this pls */
    timer_fps = 1 / ((timer_time - timer_lastup) + 0.0001f);
    timer_lastup = timer_time;
    return timer_time;
}

float skGetFPS() {
    return timer_fps;
}
