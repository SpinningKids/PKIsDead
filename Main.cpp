
#define WIN32_LEAN_AND_MEAN
#define WINDOW_CLASS_NAME "WINDOW_CLASS"

/*##########################################################*/
/* Standard includes										*/
/*##########################################################*/
//TEMP cleanme
#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Resources/resource.h"

#include "AsmMath4.h"
#include "minifmod/minifmod.h"
#else
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <unistd.h>
#include "SDL.h"
#include "glf.h"
#include "linuxresources/arial_black.h"
//#include "linuxresources/arial1.h"
#include "linuxresources/Dxn_2.h"
#include "linuxinclude/minifmod.h"

//  #define SCREENSHOOTER
#ifdef SCREENSHOOTER
#include "SDL_syswm.h"
#endif /* SCREENSHOOTER */
#endif

#include <math.h>
#include <string.h>
#include "Globals.h"
#include "GLFont.h"
#include "GLTexture.h"
#include "GenTex.h"
#include "noise.h"
#include "CoolPrint.h"
#include "ParticleSystem.h"
#include "SphereEffect.h"
#include "PKLogo.h"
#include "utils.h"

//#define caccaculo

/*##########################################################*/
/* Sound Player includes									*/
/*##########################################################*/

typedef unsigned char byte;
typedef unsigned short word;


#define NUTSH 32
#define NUTSV 16

#define QUADV 20
#define QUADH 20

GLFont *FontArial;
CParticleSystem parts1;
CParticleSystem parts2;
GLUquadricObj*   m_glqMyQuadratic;
float mytime;
bool donerendertotexture = false;
bool isMusicEnabled = true;

bool hiddenpart = false;

//vars for nuts speedup
Vector3 vnuts[NUTSH * NUTSV * 6];  //V*H*6 vertices
Vector3 nnuts[NUTSH * NUTSV * 6];  //normals
GLuint  inuts[NUTSH * NUTSV * 6];  //indexes
int vind = 0;

//vars for texturedquads speedup
Vector3 vquad[QUADV * QUADH * 4];  //V*H*6 vertices
Vector3 nquad[QUADV * QUADH * 4];  //normals
uv_coord tquad[QUADV * QUADH * 4]; //texturecoords
GLuint  iquad[QUADV * QUADH * 4];  //indexes
int qind = 0;

GLTexture* env; //genric envy
GLTexture* frame; //used by render to texture
GLTexture* frame2; //used by render to texture
GLTexture* gauss;  //DOVREBBE essere un gauss
GLTexture* toro;
GLTexture* cubo;
GLTexture *texture1;
GLTexture *texture2;
GLTexture* mytek;
GLTexture* tex;
GLTexture* tex2;
GLTexture* scritte;

GLuint logo;

//solo per provare...
Vector3 poscubo(0,0,0);
Vector3 posnut(0,0,0);
Vector3 sizecubo(0.6f,0.6f,0.4f);
//original timeline
//float timebase[18] = {0.0f, 10.705f, 11.632f, 14.54f, 17.448f, 23.273f, 29.098f, 34.7f, 35.821f, 36.348f, 47.987f, 58.458f, 72.715f, 84.340f, 95.961f, 107.617f, 119.240f, 132.0f};
// pre-panbackwards float timebase[18] = {0.0f, 10.705f, 11.632f, 14.54f, 17.084f, 23.273f, 28.734f, 34.7f, 35.821f, 36.348f, 47.987f, 58.458f, 72.715f, 84.340f, 95.961f, 107.617f, 119.240f, 132.0f};
//float timebase[19] = {0.0f, 10.705f, 11.632f, 14.54f, 17.084f, 23.273f, 28.734f, 34.7f, 35.821f, 36.348f, 47.987f, 58.458f, 72.715f, 84.340f, 95.961f, 107.617f, 117.785, 119.240f}; // 121 fu 119.240};
float timebase[19] = {0.0f, 10.705f, 11.632f, 14.54f, 17.084f, 23.273f, 28.734f, 34.7f, 35.821f, 36.348f, 47.987f, 58.458f, 72.715f, 84.340f, 95.961f, 107.617f, 117.785f, 119.240f}; // 121 fu 119.240};
//                       0        1        2       3        4        5        6      7        8        9       10       11       12       13       14

//test timeline
//float timebase[18] = {0.0f, 1.705f, 2.632f, 3.54f, 4.448f, 5.273f, 6.098f, 7.7, 8.821 ,9.348f, 47.987f, 58.458f, 72.715f, 84.340f, 95.961f, 107.617f, 119.240f, 132.0f};

/*##########################################################*/
/*Standard Var definitions :								*/
/*##########################################################*/


#ifdef WIN32
HWND		hWND;
HDC			hDC;
HGLRC		hRC;
HINSTANCE	hInstance;
#endif /* WIN32 */

/*##########################################################*/
/*Sound Player Implementation								*/
/*##########################################################*/
FMUSIC_MODULE *fmodule;
float g_delta;

#ifdef WIN32
#include <mmsystem.h>

extern "C" {
  HWAVEOUT FSOUND_WaveOutHandle;
}
#endif /* WIN32 */

float panGetTime() 
{
#ifdef WIN32
    if (isMusicEnabled)
    {
        MMTIME mmtime;
        mmtime.wType = TIME_SAMPLES;
        waveOutGetPosition(FSOUND_WaveOutHandle, &mmtime, sizeof(mmtime));
        return mmtime.u.ticks / 44100.f;
    }
#endif
    return skGetTime();
}

#define MUSIC_RES_NUM	105

typedef struct {
	int length;
	int pos;
	void *data;
} MEMFILE;

unsigned int memopen(char *name)
{
	MEMFILE *memfile;

	memfile = new MEMFILE;

#ifdef WIN32
	{	// hey look some load from resource code!
		HRSRC		rec;
		HGLOBAL		handle;

		rec = FindResourceEx(GetModuleHandle(nullptr),"RC_RTDATA", name, 0);
		handle = LoadResource(nullptr, rec);
		memfile->data = LockResource(handle);
		memfile->length = SizeofResource(nullptr, rec);
		memfile->pos = 0;
	}
#else
	memfile->data   = dixiesmod;
	memfile->length = sizeof(dixiesmod);
	memfile->pos    = 0;
#endif
	return (unsigned int)memfile;
}

void memclose(unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;
	delete memfile;
}

int memread(void *buffer, int size, unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;

	if (memfile->pos + size >= memfile->length)
		size = memfile->length - memfile->pos;

	memcpy(buffer, (char *)memfile->data+memfile->pos, size);
	memfile->pos += size;
	
	return size;
}

void memseek(unsigned int handle, int pos, signed char mode)
{
	MEMFILE *memfile = (MEMFILE *)handle;

	if (mode == SEEK_SET) 
		memfile->pos = pos;
	else if (mode == SEEK_CUR) 
		memfile->pos += pos;
	else if (mode == SEEK_END)
		memfile->pos = memfile->length + pos;

	if (memfile->pos > memfile->length)
		memfile->pos = memfile->length;
}

int memtell(unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;
	return memfile->pos;
}


//This just sets the states/matrices needed to render in a texture
void PrepareRenderToTexture(float FOV,int size)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV,((float)size)/size,0.1f,600.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glViewport(0,0,size,size);
	glLoadIdentity();
}


void DoRenderToTexture(int size,GLTexture* tex)
{
  glFlush();
	glBindTexture(GL_TEXTURE_2D,tex->getID());
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, size, size, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,WIDTH,HEIGHT);
  donerendertotexture = true;
}

//function that evaluates the pos vor a vertex on a cassini oval
Vector3 EvalNut(float t,float a,float b,int sign)
{
   Vector3 p;
   float a2 = a * a;
   float b2 = b * b;
   float a2s = a2 * sinf(2 * t);
   float c1 = b2 * b2 - a2s * a2s;
   float c2 = sqrtf(a2 * cosf(2 * t) + (c1 <= 0 ? 0 : sign * sqrtf(c1)));
   p.x = cosf(t) * c2;
   p.y = sinf(t) * c2;
   p.z = 0.0;

   if (a > b)
      p.x -= a;

   return(p);
}

//SUKA MODIFICATA QUESTA ROUTINE COPIARE TUTTA
//Just draws a cube in the origin
//Parameters:
//size      : the cube size
void dCube(const Vector3 &size)
{
    Vector3 min = -size / 2.0f;
    Vector3 max = size / 2.0f;

    glBegin(GL_QUADS);
    glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
    glNormal3f(0.0, 0.0, -1.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(min.x, min.y, min.z);
    glTexCoord2f(1.0, 0.0); glVertex3f(min.x, max.y, min.z);
    glTexCoord2f(1.0, 1.0); glVertex3f(max.x, max.y, min.z);
    glTexCoord2f(0.0, 1.0); glVertex3f(max.x, min.y, min.z);

    glNormal3f(0.0, 0.0, 1.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(min.x, min.y, max.z);
    glTexCoord2f(1.0, 0.0); glVertex3f(max.x, min.y, max.z);
    glTexCoord2f(1.0, 1.0); glVertex3f(max.x, max.y, max.z);
    glTexCoord2f(0.0, 1.0); glVertex3f(min.x, max.y, max.z);

    glNormal3f(-1.0, 0.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(min.x, min.y, max.z);
    glTexCoord2f(1.0, 0.0); glVertex3f(min.x, max.y, max.z);
    glTexCoord2f(1.0, 1.0); glVertex3f(min.x, max.y, min.z);
    glTexCoord2f(0.0, 1.0); glVertex3f(min.x, min.y, min.z);

    glNormal3f(1.0, 0.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(max.x, min.y, max.z);
    glTexCoord2f(1.0, 0.0); glVertex3f(max.x, min.y, min.z);
    glTexCoord2f(1.0, 1.0); glVertex3f(max.x, max.y, min.z);
    glTexCoord2f(0.0, 1.0); glVertex3f(max.x, max.y, max.z);

    glNormal3f(0.0, -1.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(min.x, min.y, max.z);
    glTexCoord2f(1.0, 0.0); glVertex3f(min.x, min.y, min.z);
    glTexCoord2f(1.0, 1.0); glVertex3f(max.x, min.y, min.z);
    glTexCoord2f(0.0, 1.0); glVertex3f(max.x, min.y, max.z);

    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(min.x, max.y, max.z);
    glTexCoord2f(1.0, 0.0); glVertex3f(max.x, max.y, max.z);
    glTexCoord2f(1.0, 1.0); glVertex3f(max.x, max.y, min.z);
    glTexCoord2f(0.0, 1.0); glVertex3f(min.x, max.y, min.z);
    glEnd();
}



//SUKA MODIFICATA QUESTA FUNZIONE E I SUOI PARAMETRI
//ATTENZIONE! MODIFICATE TUTTE LE CHIAMATE A dNuts (aggiunto colore tra i parametri)
//
//- Evaluate, Load Arrays and Draws the Cassini oval
//Params:
//value : distance from oval centers
//recalc : recalc the vertex arrays (to be done one time per frame)
//flatten : if enabled flatten the ovals UNDER the fval 
//fval : if flatten is enabled the ovals are flatten under this value
//
void dNuts(float value,bool recalc,bool flattenonfloor,float flattenval,rgb_a col)
{
    float theta1,theta2;
  float tstart,tstop,t1,t2;
  Vector3 p[4],q[2],n[4];
  Vector3 zperp(0,0,1);
  float a,b;  

//lod (h/r)
  float N = NUTSH;
  float M = NUTSV;

  //the calc mess is just to speed up things (sorry)
  //just call this func with calc = true once per frame

  if (recalc)
  {

    //index for arrays
    vind = 0;

    a = fabsf( sinf(value) );
    b = 1.0f;

    tstart = 0;
    if (a <= b)
      tstop = PI;
    else
      tstop = 0.5f * asinf(b*b/(a*a));

    for (int j = 0;j<(int)M;j++)     
    {
      theta1 = TWOPI * j / M;
      theta2 = TWOPI * ((j+1)%(int)M) / M;

      for (int i = 0;i<(int)N;i++) 
      { 
         int thesign = 1;
         if (a <= b) 
         {
            t1 = tstart + (tstop - tstart) * i / N;
            t2 = tstart + (tstop - tstart) * (i+1) / N;
         } 
         else 
         {
            if (i < N/2) 
            {
               t1 = tstart + 2 * (tstop - tstart) * i / N;
               t2 = tstart + 2 * (tstop - tstart) * (i+1) / N;
            } 
            else 
            {
               t1 = tstart + 2 * (tstop - tstart) * (i-N/2) / N;
               t2 = tstart + 2 * (tstop - tstart) * (i+1-N/2) / N;
               thesign = -1;
            }
         }

         // faccia dell'ovale
         p[0] = EvalNut(t1,a,b,thesign);
         q[0] = EvalNut(t1 + 0.1f/N, a, b, thesign);
         q[0] = p[0] - q[0];
         n[0] = q[0] ^ zperp; //normale 

         p[1] = EvalNut(t2,a,b,thesign);
         q[1] = EvalNut(t2 - 0.1f/N, a, b, thesign);
         q[1] = q[1] - p[1]; 
         n[1] = q[1] ^ zperp; //normale

         p[2] = p[1];
         n[2] = n[1];

         p[3] = p[0];
         n[3] = n[0];

        //the following part rotates around the oval axis
         float ct = cosf(theta1);
         float st = sinf(theta1);
         Vector3 t = p[0];

          t.x = p[0].x;
          t.y = p[0].y * ct + p[0].z * st;
          t.z = -p[0].y * st + p[0].z * ct;
          p[0] = t;

          t.x = n[0].x;
          t.y = n[0].y * ct + n[0].z * st;
          t.z = -n[0].y * st + n[0].z * ct;
          n[0] = t;

          t.x = p[1].x;
          t.y = p[1].y * ct + p[1].z * st;
          t.z = -p[1].y * st + p[1].z * ct;
          p[1] = t;

          t.x = n[1].x;
          t.y = n[1].y * ct + n[1].z * st;
          t.z = -n[1].y * st + n[1].z * ct;
          n[1] = t;

          ct = cosf(theta2);
          st = sinf(theta2);

          t.x = p[2].x;
          t.y = p[2].y * ct + p[2].z * st;
          t.z = -p[2].y * st + p[2].z * ct;
          p[2] = t;

          t.x = n[2].x;
          t.y = n[2].y * ct + n[2].z * st;
          t.z = -n[2].y * st + n[2].z * ct;
          n[2] = t;

          t.x = p[3].x;
          t.y = p[3].y * ct + p[3].z * st;
          t.z = -p[3].y * st + p[3].z * ct;
          p[3] = t;

          t.x = n[3].x;
          t.y = n[3].y * ct + n[3].z * st;
          t.z = -n[3].y * st + n[3].z * ct;
          n[3] = t;


         for (int k = 0;k<4;k++)
            n[k].Normalize();

         if (flattenonfloor)
         {
           for (int np = 0; np< 4; np++)
           {
            if (p[np].y < flattenval)
              p[np].y = flattenval;
              n[np].y = -1;
           }
         }

         if (!(p[0] == p[1]) && !(p[1] == p[2]) && !(p[2] == p[0])) 
         {
           vnuts[vind] = p[0];
           nnuts[vind] = n[0];
           inuts[vind] = vind;
           vind++;

           vnuts[vind] = p[1];
           nnuts[vind] = n[1];
           inuts[vind] = vind;
           vind++;

           vnuts[vind] = p[2];
           nnuts[vind] = n[2];
           inuts[vind] = vind;
           vind++;
/*    
           glNormal3fv((float*) &n[0]);
           glVertex3fv((float*) &p[0]);

           glNormal3fv((float*) &n[1]);
           glVertex3fv((float*) &p[1]);

           glNormal3fv((float*) &n[2]);
           glVertex3fv((float*) &p[2]);
*/    
         }
         if (!(p[0] == p[2]) && !(p[2] == p[3]) && !(p[3] == p[0])) 
         {
           vnuts[vind] = p[0];
           nnuts[vind] = n[0];
           inuts[vind] = vind;
           vind++;

           vnuts[vind] = p[2];
           nnuts[vind] = n[2];
           inuts[vind] = vind;
           vind++;

           vnuts[vind] = p[3];
           nnuts[vind] = n[3];
           inuts[vind] = vind;
           vind++;
/*    
           glNormal3fv((float*) &n[0]);
           glVertex3fv((float*) &p[0]);

           glNormal3fv((float*) &n[2]);
           glVertex3fv((float*) &p[2]);

           glNormal3fv((float*) &n[3]);
           glVertex3fv((float*) &p[3]);
*/    
         }
      }
    }
  }

  glColor4f(col.r,col.g,col.b,col.a);

  glVertexPointer(3,GL_FLOAT,0,&vnuts);
  glNormalPointer(GL_FLOAT,0,&nnuts);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  glDrawElements(GL_TRIANGLES,vind,GL_UNSIGNED_INT,&inuts);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);

}

void dHelix(float outr,float inr, int twists,int angle_steps,rgb_a startcol, rgb_a endcol)
{
    static GLuint springlist = 0;

  Vector3 vertexes[4];
  Vector3 normal;
  rgb_a col;
	
  glBegin(GL_QUADS);
	for(float phi = 0; phi <= 360; phi+=angle_steps)
	{
		for(float theta = 0; theta<=360*twists; theta+=angle_steps)
		{
			float v = phi / 180.0f * 3.142f;
			float u = theta / 180.0f * 3.142f;

			float x = cosf(u) * (inr + cosf(v)) * outr;
			float y = sinf(u) * (inr + cosf(v)) * outr;
			float z = (u - 2.0f * 3.142f + sinf(v)) * outr;

			vertexes[0].x = x;
			vertexes[0].y = y;
			vertexes[0].z = z;

            v = phi / 180.0f * 3.142f;
            u = (theta + angle_steps) / 180.0f * 3.142f;

            x = cosf(u) * (inr + cosf(v)) * outr;
            y = sinf(u) * (inr + cosf(v)) * outr;
            z = (u - 2.0f * 3.142f + sinf(v)) * outr;

			vertexes[1].x = x;
			vertexes[1].y = y;
			vertexes[1].z = z;

            v = (phi + angle_steps) / 180.0f * 3.142f;
            u = (theta + angle_steps) / 180.0f * 3.142f;

            x = cosf(u) * (inr + cosf(v)) * outr;
            y = sinf(u) * (inr + cosf(v)) * outr;
            z = (u - 2.0f * 3.142f + sinf(v)) * outr;

			vertexes[2].x = x;
			vertexes[2].y = y;
			vertexes[2].z = z;

            v = (phi + angle_steps) / 180.0f * 3.142f;
            u = theta / 180.0f * 3.142f;

            x = cosf(u) * (inr + cosf(v)) * outr;
            y = sinf(u) * (inr + cosf(v)) * outr;
            z = (u - (2.0f * 3.142f) + sinf(v)) * outr;

			vertexes[3].x = x;
			vertexes[3].y = y;
			vertexes[3].z = z;

			normal = CalcNormal(vertexes[2], vertexes[0], vertexes[1]);
      
			glNormal3fv((float*)&normal);

      col = GetFade(startcol,endcol,(theta + 1.0f) / (360.0f * (float)twists));

      glColor4fv((float*)&col);
			glVertex3fv((float*)&vertexes[0]);
			glVertex3fv((float*)&vertexes[1]);
			glVertex3fv((float*)&vertexes[2]);
			glVertex3fv((float*)&vertexes[3]);
		}
	}
	glEnd();
  
  glCallList(springlist);
	glPopMatrix();
}

// Draws the Cylinder
// Parameters:
// r : base radius
// segsh : number of revolution segments
// segsv : number of length segments
// wobble : the "wobbling" value, this is a MULTIPLIER
// wdelta : the value that will be added to the radius
// wfreq : the wobbles frequency along the tube length
void dCylinder(float r, int segsh, int segsv, float length, float wobble, float wdelta, float wfreq, float mytime, bool fill,rgb_a color,rgb_a furcolor,float furlenght)
{
  int x,y;
  float horstep = (float)TWOPI / segsh;
  float vertstep = length / segsv;
  float wob = 0;

  Vector3 tmpv0(0,0,0);
  Vector3 tmpv1(0,0,0);
  Vector3 tmpv2(0,0,0);
  Vector3 tmpv3(0,0,0);

  Vector3 norm0(0,0,0);
  Vector3 norm1(0,0,0);
  Vector3 norm2(0,0,0);
  Vector3 norm3(0,0,0);

  if (fill) {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    //glColor4f(0.9f, 0.9f, 0.9f, 0.1f);
    glColor4fv((float*)&color);

    glBegin(GL_QUADS);
  
    for (x = 0; x < segsh;x++)
    {
      for (y = 0; y < segsv;y++)
      {
          //  |_
          tmpv0.y = y * vertstep - (length / 2);
          //wob is the value tube enlarges in this segment
          wob = sinf((tmpv0.y * wfreq / length) * (float)TWOPI + wdelta) + cosf((tmpv0.y * wfreq / length) * (float)TWOPI + wdelta) * wobble;
          tmpv0.x = cosf(x * horstep) * (r + wob);
          tmpv0.z = sinf(x * horstep) * (r + wob);
          norm0.x = sinf(x * horstep);
          norm0.y = 0.0f;
          norm0.z = -cosf(x * horstep);
          //glNormal3f( sinf(x * horstep), 0.0f, -cosf(x * horstep));

          //   _
          //  |
          tmpv1.y = (y+1) * vertstep - (length / 2);
          wob = sinf((tmpv1.y * wfreq / length) * (float)TWOPI + wdelta)  + cosf((tmpv1.y * wfreq / length) * (float)TWOPI + wdelta) * wobble;
          tmpv1.x = cosf(x * horstep) * (r + wob);
          tmpv1.z = sinf(x * horstep) * (r + wob);
          norm1.x = sinf(x * horstep);
          norm1.y = 0.0f;
          norm1.z = -cosf(x * horstep);

          //   _
          //    |
          tmpv2.y = (y+1) * vertstep - (length / 2);
          wob = sinf((tmpv2.y * wfreq / length) * (float)TWOPI + wdelta)  + cosf((tmpv2.y * wfreq / length) * (float)TWOPI + wdelta) * wobble;
          tmpv2.x = cosf((x+1) * horstep) * (r + wob);
          tmpv2.z = sinf((x+1) * horstep) * (r + wob);
          norm2.x = sinf((x+1) * horstep);
          norm2.y = 0.0f;
          norm2.z = -cosf((x+1) * horstep);
          //glNormal3f( sinf((x+1) * horstep), 0, -cosf((x+1) * horstep));

          //  _|
          tmpv3.y = y * vertstep  - (length / 2);
          wob = sinf((tmpv3.y * wfreq / length) * (float)TWOPI + wdelta)  + cosf((tmpv3.y * wfreq / length) * (float)TWOPI + wdelta) * wobble;
          tmpv3.x = cosf((x+1) * horstep) * (r + wob);
          tmpv3.z = sinf((x+1) * horstep) * (r + wob);
          norm3.x = sinf((x+1) * horstep);
          norm3.y = 0.0f;
          norm3.z = -cosf((x+1) * horstep);
          //glNormal3f( sinf((x+1) * horstep), 0, -cosf((x+1) * horstep));

          glNormal3fv((float*)&norm0);
          glVertex3fv((float*)&tmpv0);

          glNormal3fv((float*)&norm1);
          glVertex3fv((float*)&tmpv1);

          glNormal3fv((float*)&norm2);
          glVertex3fv((float*)&tmpv2);

          glNormal3fv((float*)&norm3);
          glVertex3fv((float*)&tmpv3);
      }
    }

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
  }

  int ch = segsh*4;
  int cv = segsv/2;
  for (int ix = 0; ix < ch;ix++)
  {
    for (int iy = 0; iy < cv;iy++)
    {
        float x = (TWOPI*(vnoise(mytime,vlattice(ix, iy, 1))+ix))/ch;
        float y = (length*(vnoise(mytime,vlattice(ix, iy, 2))+iy))/cv;
        //  |_
        tmpv0.y = y  - (length / 2.0f);
        wob = sinf((tmpv0.y * wfreq / length) * (float)TWOPI + wdelta) + cosf((tmpv0.y * wfreq / length) * (float)TWOPI + wdelta) * wobble;
        tmpv0.x = cosf(x) * (r + wob);
        tmpv0.z = sinf(x) * (r + wob);
        glColor4f(furcolor.r,furcolor.g,furcolor.b,0.5f);
        glVertex3fv((float*)&tmpv0);

        //  |_
        tmpv1.y = y - (length / 2);
        wob = sinf((tmpv1.y * wfreq / length) * (float)TWOPI + wdelta) + cosf((tmpv1.y * wfreq / length) * (float)TWOPI + wdelta) * wobble;
        tmpv1.y += wfreq*TWOPI*cosf(tmpv1.y*wfreq*TWOPI/length)/length - wfreq*TWOPI*wobble*sinf(tmpv1.y*wfreq*TWOPI/length)/length;
        tmpv1.x = cosf(x) * ((r*furlenght) + wob);
        tmpv1.z = sinf(x) * ((r*furlenght) + wob);
        glColor4f(furcolor.r,furcolor.g,furcolor.b,0.0f);
        glVertex3fv((float*)&tmpv1);
    }
  }

  glEnd();

}

//SUKA AGGIUNTA TUTTA QUESTA ROUTINE
uv_coord TwirlTexCoords(int x, int y,float value, float divisor)
{
    float DIVS_X = QUADV; //m_iLod - (m_iLod/2);
    float DIVS_Y = QUADH; //m_iLod;

    float r = sqrtf((x - DIVS_X / 2) * (x - DIVS_X / 2) + (y - DIVS_Y / 2) * (y - DIVS_Y / 2));
    float a = atan2f((x - DIVS_X / 2), (y - DIVS_Y / 2));
    float r2 = r + 5 * (1 + sinf(value * 3));
    float a2 = value + a + 0.5f * sinf(r / 8 + value * 1.5f) + 0.55f * cosf(r / 4 + value * 1.9f);
    float tx = 0.75f * r2 * sinf(a2) / DIVS_X;
    float ty = 0.75f * r2 * cosf(a2) / DIVS_Y;
    return uv_coord(tx - 0.5f, ty - 0.5f);

}
//SUKA FINE AGGIUNTA


//draws a deformed torus
//Parems :
// c : center
// r0 : outher radius
// r1 : section radius
// n : number of revolution segments
// theta1 : starting revolution angle (along revolution path)
// theta2 : ending revolution angle
// phi : starting of segments angle (along section circle)
//phi2 : ending of segments angle
//wobbleval : value for deformation
//deform : enables the deformation
void dTorus(float time, const Vector3& c, float r0, float r1, int n, float theta1, float theta2, float phi1, float phi2, bool deform)
{
    Vector3 e0, e1, p0, p1;
    uv_coord t;
    float tmpr1;


    if (r1 < 0)
        r1 = -r1;
    if (r0 < 0)
        r0 = -r0;
    if (n < 0)
        n = -n;

    for (int j = 0; j < n; j++)
    {
        float t1 = j * (theta2 - theta1) / n;
        float t2 = (j + 1) * (theta2 - theta1) / n;

        glBegin(GL_QUAD_STRIP);

        for (int i = 0; i <= n; i++)
        {

            float phi = phi1 + i * (phi2 - phi1) / n;

            if (deform)
                tmpr1 = r1 + sinf((float)i) / 5 + sinf(t2) * (1 + sinf(j / 8.f + time * 3));
            //tmpr1 = r1 + sin(i)/8;
            else
                tmpr1 = r1;

            e0.x = cosf(t1) * tmpr1 * cosf(phi);
            e0.y = sinf(phi) * tmpr1;
            e0.z = sinf(t1) * tmpr1 * cosf(phi);
            e0.Normalize();

            p0.x = c.x + cosf(t1) * (r0 + tmpr1 * cosf(phi));
            p0.y = c.y + sinf(phi) * tmpr1;
            p0.z = c.z + sinf(t1) * (r0 + tmpr1 * cosf(phi));

            t.u = i / (float)n;
            t.v = j / (float)n;


            e1.x = cosf(t2) * tmpr1 * cosf(phi);
            e1.y = sinf(phi) * tmpr1;
            e1.z = sinf(t2) * tmpr1 * cosf(phi);
            e1.Normalize();

            p1.x = cosf(t2) * (r0 + tmpr1 * cosf(phi));
            p1.y = sinf(phi) * tmpr1;
            p1.z = sinf(t2) * (r0 + tmpr1 * cosf(phi));

            t.u = i / (float)n;
            t.v = (j + 1) / (float)n; // BUG: This should be moved after glTexCoord2f

            glNormal3f(e0.x, e0.y, e0.z);
            glTexCoord2f(t.u, t.v);
            glVertex3f(p0.x, p0.y, p0.z + sinf(p0.x) * cosf(time) * 2.0f);

            glNormal3f(e1.x, e1.y, e1.z);
            glTexCoord2f(t.u, t.v);
            glVertex3f(p1.x, p1.y, p1.z + sinf(p1.x) * cosf(time) * 2.0f);

        }
        glEnd();
    }
}


// Return a texture coord displaved like a wave by a JLSAC
//x,y : original texture coords
// value : waving value (time)
// divisor : the resulting wave sin/cos displacement is divided by this value
uv_coord WaterTexCoords(int x, int y, float value, float divisor)
{
    float tx, ty;

    float DIVS_X = QUADV; //m_iLod - (m_iLod/2);
    float DIVS_Y = QUADH; //m_iLod;

    tx = (x / DIVS_X) + sinf(x * value) / divisor;
    ty = (y / DIVS_Y) + cosf(y * value) / divisor;

    if (tx > 1)
        tx = 1;

    if (ty > 1)
        ty = 1;

    return uv_coord(tx, ty);
    //glTexCoord2f(tx,ty);

}

void drawHelix(float angle, float inr, float outr, int twirls, int angstep, GLenum mode, rgb_a startcol, rgb_a endcol)
{

    glEnable(GL_COLOR_MATERIAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    if (mode == GL_LINE)
        glLineWidth(2);

    glPolygonMode(GL_FRONT_AND_BACK, mode);

    glPushMatrix();												// Push The Modelview Matrix
    glLoadIdentity();											// Reset The Modelview Matrix
    //glRotatef(180,1,0,0);
    //glTranslatef(0,0,10);
    glTranslatef(0, 0, -50);
    glRotatef(angle, 0, 0, 1);
    //I've created the list in the init function
    dHelix(inr, outr, twirls, angstep, startcol, endcol);
    glPopMatrix();

    if (mode == GL_LINE)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glLineWidth(1);
    }
}

//SUKA MODIFICATA LA SEGUENTE ROUTINE TUTTA !
//MODIFICATE ANCHE TUTTE LE SUE CHIAMATE NEL CODICE
//Draws a TexturedQuad with a waving effect on it
//Params:
//tex : a GLTexture object
// alpha : the alpha
// sx : the scale along x of the TEXTURE coords
// sy : the scale along Y of the TEXTURE coords
// calc : does the vertex array needs to be reloaded ? (once per frame)
// value : the value used to wave the texture
// divisor : the wave size divisor
void drawTexture(GLTexture* tex, float sx, float sy, bool calc, float value, float divisor, rgb_a color, bool water)
{
    if (color.a <= 0)
        return;

    glDepthMask(GL_FALSE);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_CULL_FACE);
    glDisable(GL_COLOR_MATERIAL);
    tex->use();

    glPushMatrix();
    glLoadIdentity();
    panViewOrtho();
    glScalef(sx, sy, 1);
    glColor4f(color.r, color.g, color.b, color.a);

    //the calc mess is just used to speed up quadwavedtexture rendering...    
    //if calc=true then it laods the array, else just draws the array
    if (calc)
    {
        qind = 0;
        float DIVS_X = QUADH; //m_iLod - (m_iLod/2);
        float DIVS_Y = QUADV; //m_iLod;

        for (int y = 0; y < DIVS_Y; y++) {
            for (int x = 0; x < DIVS_X; x++) {
                if (water)
                    tquad[qind] = WaterTexCoords(x, -y, value, divisor);
                else
                    tquad[qind] = TwirlTexCoords(x, -y, value, divisor);

                vquad[qind] = Vector3((float)x / DIVS_X * (float)WIDTH, (float)y / DIVS_Y * (float)HEIGHT, 0);
                iquad[qind] = qind;
                qind++;

                if (water)
                    tquad[qind] = WaterTexCoords(x + 1, -y, value, divisor);
                else
                    tquad[qind] = TwirlTexCoords(x + 1, -y, value, divisor);

                vquad[qind] = Vector3((float)(x + 1) / DIVS_X * (float)WIDTH, (float)y / DIVS_Y * (float)HEIGHT, 0);
                iquad[qind] = qind;
                qind++;

                if (water)
                    tquad[qind] = WaterTexCoords(x + 1, -(y + 1), value, divisor);
                else
                    tquad[qind] = TwirlTexCoords(x + 1, -(y + 1), value, divisor);

                vquad[qind] = Vector3((float)(x + 1) / DIVS_X * (float)WIDTH, (float)(y + 1) / DIVS_Y * (float)HEIGHT, 0);
                iquad[qind] = qind;
                qind++;

                if (water)
                    tquad[qind] = WaterTexCoords(x, -(y + 1), value, divisor);
                else
                    tquad[qind] = TwirlTexCoords(x, -(y + 1), value, divisor);

                vquad[qind] = Vector3((float)x / DIVS_X * (float)WIDTH, (float)(y + 1) / DIVS_Y * (float)HEIGHT, 0);
                iquad[qind] = qind;
                qind++;
            }
        }
    }

    glVertexPointer(3, GL_FLOAT, 0, &vquad);
    glTexCoordPointer(2, GL_FLOAT, 0, &tquad);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glDrawElements(GL_QUADS, qind - 1, GL_UNSIGNED_INT, &iquad);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glPopMatrix();

    glEnable(GL_COLOR_MATERIAL);

}

//draws the Cassini Oval scene
void drawNuts(float t)
{
    env->use();

    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);  //##############
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glPushMatrix();
    panViewPerspective();

    glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
    //set a amtrix to render them slighty rotated and 
    //translated going far away (IMHO looks good)
    glTranslatef(-2.2f, -0.3f, 0.0f); // pan modifica - se sta sempre dentro lo schermo fa da cagare...
    glRotatef(-30, 0, 1, 0);
    glRotatef(sinf(t * 2) * 30.0f, 0.0f, 0.0f, 1.0f);
    glTranslatef(0, 0, -3);
    dNuts(t, true, false, 0, rgb_a(1, 1, 1, 0.4f));  //just loads the vertexes once


    //glTranslatef(0,0,-5);
    glPushMatrix();
    glScalef(0.9f, 0.9f, 0.9f);
    dNuts(t, false, false, 0, rgb_a(1, 1, 1, 0.4f));
    glPopMatrix();

    glPushMatrix();
    glScalef(0.8f, 0.8f, 0.8f);
    dNuts(t, false, false, 0, rgb_a(1, 1, 1, 0.4f));
    glPopMatrix();

    glPopMatrix();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);

}

//SUKA MODIFICA questa routine e' stata TUTTA modificata
//AGGIUNTO PASSAGGIO DEL COLORE, MODIFICATE TUTTE LE SUE CHIAMATE
//draws the Tube scene
void drawTubo(int order,float t,rgb_a coltubo,rgb_a colpeli)
{
  toro->use();

  glDepthMask(GL_TRUE);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE);
  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);


  constexpr float r = 2.0f;
  constexpr int segsh = 16;
  constexpr int segsv = 80;
  constexpr float length = 80.0f;
  constexpr float wobblepar = 0.5f;
  constexpr float freq = 10.0f;

  float deltaw = t * 6.0f;


  glEnable(GL_COLOR_MATERIAL);
  glPushMatrix();
    glPushMatrix();
      glTranslatef(0.0f, 0.0f, -10.0f);
      glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
      glRotatef(sinf(t) * 15.0f, 0.0f, 0.0f, 1.0f);
      glRotatef(sinf(t) * 15.0f, 1.0f, 0.0f, 0.0f);
      dCylinder(r,segsh,segsv,length,wobblepar,deltaw,freq, t, true,coltubo,colpeli,1.5f);

    glPopMatrix();

    glPushMatrix();
      glTranslatef(0.0f, 0.0f, -10.0f);
      glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
      glRotatef(sinf(t + 0.2f) * 15.0f, 0.0f, 0.0f, 1.0f);
      glRotatef(sinf(t + 0.2f) * 15.0f, 1.0f, 0.0f, 0.0f);
      dCylinder(r,segsh,segsv,length,wobblepar,deltaw,freq, t, true,coltubo,colpeli,1.5f);
    glPopMatrix();

    glPushMatrix();
      glTranslatef(0.0f, 0.0f, -10.0f);
      glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
      glRotatef(sinf(t + 0.4f) * 15.0f, 0.0f, 0.0f, 1.0f);
      glRotatef(sinf(t + 0.4f) * 15.0f, 1.0f, 0.0f, 0.0f);
      dCylinder(r,segsh,segsv,length,wobblepar,deltaw,freq, t, true, coltubo,colpeli,1.5f);
    glPopMatrix();
    
  glPopMatrix();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
  glCullFace(GL_BACK);

}

//draws teh Torus scene
void drawToroide(int order, float t, float mytime) // porcodio, se e' standard mettici mytime!
{
  toro->use();

  glDepthMask(GL_TRUE);
  glEnable(GL_BLEND);
  //glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  glCullFace(GL_FRONT);
  glEnable(GL_CULL_FACE);

  glPushMatrix();

    if (order < 1)
      glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
    else //if (order == 2)
      glColor4f(1.0f, 1.0f, 1.0f, 0.2f - 0.5f*((panGetTime() - 11.2f) / 5.0f));

    glDisable(GL_DEPTH_TEST);
    glTranslatef(0,0,-10);
    glRotatef(sinf(t * 1.2f) * 140.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(sinf(t * 1.7f) * 60.0f, 0.0f, 0.0f, 1.0f);
    dTorus(mytime,Vector3(0.0f,0.0f,0.0f),2.0f,0.6f,64,0,(float)TWOPI,0.0f,(float)TWOPI,true);
    glTranslatef(0.0f, 0.0f, 0.01f);
    if (order < 2)
      glColor4f(1.0f, 1.0f, 1.0f, 0.1f);
    else
      glColor4f(1.0f, 1.0f, 1.0f, (0.5f*(0.2f - t)));
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    dTorus(mytime, Vector3(0,0,0),2.0f,0.6f,64,0,(float)TWOPI,0.0f,(float)TWOPI,true);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

  glPopMatrix();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glCullFace(GL_BACK);
  //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  //glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}


void drawWissEffect(const Vector3 &pos, const Vector3 &rot, const rgb_a &pcolor, const rgb_a &vcolor, bool base)
{
	float fogColor[4] = { 0, 0, 0, 0 };

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  // fog stuff
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf( GL_FOG_DENSITY, 0.01f ); 
	glFogf(GL_FOG_START, -pos.z-3.0f);
	glFogf(GL_FOG_END, -pos.z);
	glHint(GL_FOG_HINT, GL_FASTEST);
	glEnable(GL_FOG);

	glBlendFunc( GL_SRC_ALPHA, GL_ONE);
  panViewPerspectiveFOV(45);

  glPushMatrix();
  glLoadIdentity();
	//glTranslatef (0.0f, 0.0f, -15.0f);							// Translate 6 Units Into The Screen
  glTranslatef(pos.x,pos.y,pos.z);
  glRotatef(rot.x ,1,0,0);
  glRotatef(rot.y ,0,1,0);
  glRotatef(rot.z ,0,0,1);
  dSphereEffect(pcolor,vcolor,base);
  glPopMatrix();

  glPushMatrix();
  glLoadIdentity();
  glTranslatef(pos.x,pos.y,pos.z);
  glRotatef(rot.x ,1,0,0);
  glRotatef(rot.y ,0,1,0);
  glRotatef(rot.z ,0,0,1);
  glRotatef(90,0,1,0);
  dSphereEffect(pcolor,vcolor,base);
  glPopMatrix();

  glPushMatrix();
  glLoadIdentity();
  glTranslatef(pos.x,pos.y,pos.z);
  glRotatef(rot.x ,1,0,0);
  glRotatef(rot.y ,0,1,0);
  glRotatef(rot.z ,0,0,1);
  glRotatef(90,1,0,0);
  dSphereEffect(pcolor,vcolor,base);
  glPopMatrix();

	glDisable(GL_FOG);
}

//Draws a sphere, with an exaggerated FOV
//Parameters :
// rot : rotation
// col : colour
void drawSfondo(const Vector3 &rot, const rgb_a &col)
{
  cubo->use();

   glPushMatrix();
      glLoadIdentity();

      panViewPerspectiveFOV(168);

      glScalef(2,2,2);
	    glDisable(GL_TEXTURE_GEN_S);
	    glDisable(GL_TEXTURE_GEN_T);
      glDepthMask(GL_FALSE);
      glDisable(GL_LIGHTING);
      glDisable(GL_DEPTH_TEST);
      glEnable(GL_TEXTURE_2D);
      glDisable(GL_CULL_FACE);
      glDisable(GL_BLEND);
      glBindTexture(GL_TEXTURE_2D, cubo->getID());
        glTranslatef(0, 0, 0);
        glRotatef(rot.x,1,0,0);
        glRotatef(rot.y,0,1,0);
        glRotatef(rot.z,0,0,1);
        glColor4f(col.r,col.g,col.b,col.a);
        gluSphere(m_glqMyQuadratic,1.3,32,32);
      glDepthMask(GL_TRUE);
  glPopMatrix();

  panViewPerspectiveFOV(45);
}


//Draw a border on top/bottom of the screen
// bcol : colour used for the border side
// ccolor : colour used for the central side
// size : size of borders
void drawBorder(rgb_a bcol, rgb_a ccol, float size)
{
  glPushMatrix();
  panViewOrtho();
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHTING);
  glDisable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glBegin(GL_QUADS);
    glColor4f(bcol.r,bcol.g,bcol.b,bcol.a);
    glVertex3f(0,HEIGHT,-1);
    glVertex3f(WIDTH,HEIGHT,-1);

    glColor4f(ccol.r,ccol.g,ccol.b,ccol.a);
    glVertex3f(WIDTH,HEIGHT - size,-1);
    glVertex3f(0,HEIGHT - size,-1);

    glColor4f(ccol.r,ccol.g,ccol.b,ccol.a);
    glVertex3f(0,size,-1);
    glVertex3f(WIDTH,size,-1);

    glColor4f(bcol.r,bcol.g,bcol.b,bcol.a);
    glVertex3f(WIDTH,0,-1);
    glVertex3f(0,0,-1);
  glEnd();
  glPopMatrix();
}



//Draws a single textured quad all over the screen
//parameters :
// col : material color used
// sx : scale along x
// sy : scale along y
// modes : alpha source mode (to be passed at glBlendFunc)
// moded : alpha dest mode (to be passed at glBlendFunc)
// tex : the texture object* to be used
void drawBlend(rgb_a col, int sx, int sy, int ex, int ey, GLenum modes, GLenum moded, GLTexture* tex)
{

  if (col.a <= 0)
      return;

  panViewPerspective();

  glDepthMask(GL_FALSE);

  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  if (tex != nullptr)
  {
    glEnable(GL_TEXTURE_2D);
    tex->use();
  }

  glBlendFunc(modes,moded);
  glEnable(GL_BLEND);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_CULL_FACE);
  glDisable(GL_COLOR_MATERIAL);

  glPushMatrix();

  panViewOrtho();
  glBegin(GL_QUADS);
    glColor4f(col.r,col.g,col.b,col.a);
    glTexCoord2f(0,0);
    glVertex3f(sx,sy,-0.1f);

    glTexCoord2f(1,0);
    glVertex3f(ex,sy,-0.1f);

    glTexCoord2f(1,1);
    glVertex3f(ex,ey,-0.1f);

    glTexCoord2f(0,1);
    glVertex3f(sx,ey,-0.1f);
  glEnd();

  glPopMatrix();

  glBlendFunc(GL_SRC_ALPHA,GL_ONE);
  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_COLOR_MATERIAL);

}


#define MAXSEG 20
void drawBlendBis(float t, float power,rgb_a col, int sx, int sy, int ex, int ey, int seg, float disp, GLenum modes, GLenum moded, GLTexture* tex)
{

  if (seg >= MAXSEG) seg = MAXSEG-1;
  static float posx[MAXSEG][MAXSEG];
  static float posy[MAXSEG][MAXSEG];
  static float module[MAXSEG][MAXSEG];
  if (col.a <= 0)
      return;

  panViewPerspective();

  glDepthMask(GL_FALSE);

  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  if (tex != nullptr)
  {
    glEnable(GL_TEXTURE_2D);
    tex->use();
  }

  glBlendFunc(modes,moded);
  glEnable(GL_BLEND);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_CULL_FACE);
  glDisable(GL_COLOR_MATERIAL);

  glPushMatrix();

  panViewOrtho();
  glBegin(GL_QUADS);

  glColor4f(col.r,col.g,col.b,col.a);
  float step = 1.f/seg;
  float dx = (ex-sx)*step;
  float dy = (ey-sy)*step;
  int i, j;
  for(i = 0; i <= seg; i++)
    for(j = 0; j <= seg; j++) {
      float a = vnoise(i*0.25f, j*0.25f, t);
      float b = vnoise(i*0.25f, j*0.25f, t+1000);
      posx[i][j] = sx+dx*i+disp*powf(a, power)*((a>0)?1:-1);
      posy[i][j] = sy+dy*j+disp*powf(b, power)*((b>0)?1:-1);
      module[i][j] = col.a*(2+a*a+b*b)/4;
    }
  for(i = 0; i < seg; i++) {
    for(j = 0; j < seg; j++) {
      glColor4f(col.r,col.g,col.b,module[i][j]);
      glTexCoord2f(step*i,step*j);
      glVertex3f(posx[i][j],posy[i][j],-0.1f);

      glColor4f(col.r,col.g,col.b,module[i+1][j]);
      glTexCoord2f(step*(i+1),step*j);
      glVertex3f(posx[i+1][j],posy[i+1][j],-0.1f);

      glColor4f(col.r,col.g,col.b,module[i+1][j+1]);
      glTexCoord2f(step*(i+1),step*(j+1));
      glVertex3f(posx[i+1][j+1],posy[i+1][j+1],-0.1f);

      glColor4f(col.r,col.g,col.b,module[i][j+1]);
      glTexCoord2f(step*i,step*(j+1));
      glVertex3f(posx[i][j+1],posy[i][j+1],-0.1f);
    }
  }
  glEnd();

  glPopMatrix();

  glBlendFunc(GL_SRC_ALPHA,GL_ONE);
  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_COLOR_MATERIAL);

}

//da finire merda, non funziona, per ora toglila 'sta sena PUTTANAZZA

//Just draw some vertical JLSAC lines on the screen
void drawLines(float t,float alpha, int n)
{
  glDepthMask(GL_FALSE);
  glPushMatrix();
  panViewPerspective();
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE);
  glBegin(GL_LINES);
    glColor4f(1.0f , 0.6f, 0.3f, alpha);
    for (int i = 0; i < n; i++)
    {
      float spd = vlattice(i, 1)*10;
      float phs = vlattice(i, 2)*10;
      glVertex3f(cosf(t/2*spd) * sinf(t/2 + phs) * 5,-100,-10);
      glVertex3f(cosf(t/2*spd) * sinf(t/2 + phs) * 5,100,-10);
    }
  glEnd();

  glPopMatrix();
}

//SUKA aggiunta questa funzione
void drawBugs(float t,rgb_a barcolor,Vector3 pos,Vector3 rot,Vector3 size,float nutpos,Vector3 eye)
{
  glClearColor(0.3f,0.2f,0.1f,0.5f);

	float fogColor[4] = { 0.3f, 0.2f, 0.1f, 0.f };

  panViewPerspectiveFOV(45.0f);
  gluLookAt(eye.x,eye.y,eye.z,0,0,-1,0,1,0);
  // fog stuff
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf( GL_FOG_DENSITY, 0.01f ); 
	glFogf(GL_FOG_START, 6);
	glFogf(GL_FOG_END, 22);
	glHint(GL_FOG_HINT, GL_FASTEST);
	glEnable(GL_FOG);
  
  glTranslatef(pos.x,pos.y,pos.z);
  glRotatef(rot.x,1,0,0);
  glRotatef(rot.y,0,1,0);
  glRotatef(rot.z,0,0,1);

  glEnable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_COLOR_MATERIAL);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
  glDisable(GL_TEXTURE_2D);

  glPushMatrix();
  glColor4fv((float*)&barcolor);
  dCube(size);
  glPopMatrix();

  glDisable(GL_FOG);
  glDisable(GL_LIGHTING);


  glBlendFunc(GL_SRC_ALPHA,GL_ONE);
  glEnable(GL_BLEND);
  glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  glDisable(GL_CULL_FACE); 
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);  //##############
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  env->use();
  glPushMatrix();
    panViewPerspective();
    gluLookAt(eye.x,eye.y,eye.z,0,0,-1,0,1,0);
    glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
    glTranslatef(pos.x,pos.y + (size.y / 2.0f),pos.z);
    glRotatef(rot.x,1,0,0);
    glRotatef(rot.y + 90,0,1,0);
    glRotatef(rot.z,0,0,1);

    glPushMatrix();
    glTranslatef(nutpos - 1.9f,0,1.3f);
    glScalef(0.6f,0.6f,0.6f);
    dNuts(t,true,true,-0.3f,rgb_a(1,1,1,0.9f));  //just loads the vertexes once
    glScalef(1.0f,-1.0f,1.0f);
    glTranslatef(0.f,0.5f,0.f);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.1f));  //just loads the vertexes once
    glTranslatef(0.f,-0.5f,0.f);
    glScalef(1.0f,0.001f,1.0f);
    glTranslatef(-0.5f,-20.0f,-0.5f);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    dNuts(t,false,true,-0.3f,rgb_a(0.0f, 0.0f, 0.0f, 0.05f));  //just loads the vertexes once
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(nutpos,0,0.3f);
    glScalef(0.6f,0.6f,0.6f);
    dNuts(t,true,true,-0.3f,rgb_a(1,1,1,0.8f));  //just loads the vertexes once
    glScalef(1.0f,-1.0f,1.0f);
    glTranslatef(0.f,0.5f,0.f);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.1f));  //just loads the vertexes once
    glTranslatef(0.f,-0.5f,0.f);
    glScalef(1.0f,0.001f,1.0f);
    glTranslatef(-0.5f,-20.0f,-0.5f);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    dNuts(t,false,true,-0.3f,rgb_a(0.0, 0.0, 0.0, 0.05f));  //just loads the vertexes once
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(nutpos + 1.6f,0.f,-1.6f);
    glScalef(0.5f,0.5f,0.5f);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.8f));  //just loads the vertexes once
    glScalef(1.0f,-1.0f,1.0f);
    glTranslatef(0.f,0.5f,0.f);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.1f));  //just loads the vertexes once
    glTranslatef(0.f,-0.5f,0.f);
    glScalef(1.0f,0.001f,1.0f);
    glTranslatef(-0.7f,-1.0f,-0.5f);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    dNuts(t,false,true,-0.3f,rgb_a(0.0f, 0.0f, 0.0f, 0.05f));  //just loads the vertexes once
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(nutpos + 2.6f,0.f,2.5f);
    glScalef(0.5f,0.5f,0.5f);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.8f));  //just loads the vertexes once
    glScalef(1.0f,-1.0f,1.0f);
    glTranslatef(0.f,0.5f,0.f);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.1f));  //just loads the vertexes once
    glTranslatef(0.f,-0.5f,0.f);
    glScalef(1.0f,0.001f,1.0f);
    glTranslatef(-0.7f,-1.0f,-0.5f);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    dNuts(t,false,true,-0.3f,rgb_a(0.0, 0.0, 0.0, 0.05f));  //just loads the vertexes once
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(nutpos + 3.5f,0,0.5f);
    glScalef(0.6f,0.6f,0.6f);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.8f));  //just loads the vertexes once
    glScalef(1.0,-1.0,1.0);
    glTranslatef(0,0.5,0);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.1f));  //just loads the vertexes once
    glTranslatef(0,-0.5,0);
    glScalef(1.0,0.001f,1.0);
    glTranslatef(-0.7f,-1.0,-0.5);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    dNuts(t,false,true,-0.3f,rgb_a(0.0, 0.0, 0.0, 0.05f));  //just loads the vertexes once
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(nutpos + 4.5f,0,-1.5f);
    glScalef(0.7f,0.7f,0.7f);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.8f));  //just loads the vertexes once
    glScalef(1.0,-1.0,1.0);
    glTranslatef(0,0.5,0);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.1f));  //just loads the vertexes once
    glTranslatef(0,-0.5,0);
    glScalef(1.0,0.001f,1.0);
    glTranslatef(-0.7f,-1.0,-0.5);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    dNuts(t,false,true,-0.3f,rgb_a(0.0, 0.0, 0.0, 0.05f));  //just loads the vertexes once
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(nutpos + 6.1f,0,-0.5f);
    glScalef(0.5,0.5,0.5);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.8f));  //just loads the vertexes once
    glScalef(1.0,-1.0,1.0);
    glTranslatef(0,0.5,0);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.1f));  //just loads the vertexes once
    glTranslatef(0,-0.5,0);
    glScalef(1.0,0.001f,1.0);
    glTranslatef(-0.7f,-1.0,-0.5);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    dNuts(t,false,true,-0.3f,rgb_a(0.0, 0.0, 0.0, 0.05f));  //just loads the vertexes once
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(nutpos + 7.7f,0,-2.0);
    glScalef(0.5,0.5,0.5);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.8f));  //just loads the vertexes once
    glScalef(1.0,-1.0,1.0);
    glTranslatef(0,0.5,0);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.1f));  //just loads the vertexes once
    glTranslatef(0,-0.5,0);
    glScalef(1.0,0.001f,1.0);
    glTranslatef(-0.7f,-1.0,-0.5);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    dNuts(t,false,true,-0.3f,rgb_a(0.0, 0.0, 0.0, 0.05f));  //just loads the vertexes once
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(nutpos + 8.1f,0,-0.3f);
    glScalef(0.5,0.5,0.5);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.8f));  //just loads the vertexes once
    glScalef(1.0,-1.0,1.0);
    glTranslatef(0,0.5,0);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.1f));  //just loads the vertexes once
    glTranslatef(0,-0.5,0);
    glScalef(1.0,0.001f,1.0);
    glTranslatef(-0.7f,-1.0,-0.5);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    dNuts(t,false,true,-0.3f,rgb_a(0.0, 0.0, 0.0, 0.05f));  //just loads the vertexes once
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(nutpos + 8.6f,0,2.3f);
    glScalef(0.6f,0.6f,0.6f);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.8f));  //just loads the vertexes once
    glScalef(1.0,-1.0,1.0);
    glTranslatef(0,0.5,0);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.1f));  //just loads the vertexes once
    glTranslatef(0,-0.5,0);
    glScalef(1.0,0.001f,1.0);
    glTranslatef(-0.7f,-1.0,-0.5);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    dNuts(t,false,true,-0.3f,rgb_a(0.0, 0.0, 0.0, 0.05f));  //just loads the vertexes once
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(nutpos + 9.1f,0,-2.3f);
    glScalef(0.6f,0.6f,0.6f);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.8f));  //just loads the vertexes once
    glScalef(1.0,-1.0,1.0);
    glTranslatef(0,0.5,0);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.1f));  //just loads the vertexes once
    glTranslatef(0,-0.5,0);
    glScalef(1.0,0.001f,1.0);
    glTranslatef(-0.7f,-1.0,-0.5);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    dNuts(t,false,true,-0.3f,rgb_a(0.0, 0.0, 0.0, 0.05f));  //just loads the vertexes once
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(nutpos + 9.4f,0,0.5);
    glScalef(0.5,0.5,0.5);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.8f));  //just loads the vertexes once
    glScalef(1.0,-1.0,1.0);
    glTranslatef(0,0.5,0);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.1f));  //just loads the vertexes once
    glTranslatef(0,-0.5,0);
    glScalef(1.0,0.001f,1.0);
    glTranslatef(-0.7f,-1.0,-0.5);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    dNuts(t,false,true,-0.3f,rgb_a(0.0, 0.0, 0.0, 0.05f));  //just loads the vertexes once
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(nutpos + 9.6f,0,1.5f);
    glScalef(0.6f,0.6f,0.6f);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.8f));  //just loads the vertexes once
    glScalef(1.0,-1.0,1.0);
    glTranslatef(0,0.5,0);
    dNuts(t,false,true,-0.3f,rgb_a(1,1,1,0.1f));  //just loads the vertexes once
    glTranslatef(0,-0.5,0);
    glScalef(1.0,0.001f,1.0);
    glTranslatef(-0.7f,-1.0,-0.5);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    dNuts(t,false,true,-0.3f,rgb_a(0.0, 0.0, 0.0, 0.05f));  //just loads the vertexes once
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glPopMatrix();

  glPopMatrix();

}

//Draws the particle system
//Params :
// t : elapsed time
// pos : emitter position
// velocity : particles velocity
// gravity : system gravity
// rot : system rotation
// partstartsize : particle size when born
// partendsize : particle size when dying
// createnew : how many particles needs to be created this frame
void drawParticles(float t, const Vector3 &pos, const Vector3 &velocity, const Vector3 &gravity, const Vector3 &rot, float partstartsize, float partendsize, int createnew)
{
  //}
}

void drawCreditsBack(float t) {
    drawBlendBis(t / 1.454875f * 4, 8, rgb_a(1, 1, 1, 0.25f), -100, -75, 740, 555, 20, 20, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, nullptr);
    drawBlendBis(t / 1.454875f * 4 - 0.2f, 4, rgb_a(1, 1, 1, 0.125f), -100, -75, 740, 555, 20, 30, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, nullptr);
    drawBlendBis(t / 1.454875f * 4 - 0.4f, 2, rgb_a(1, 1, 1, 0.0625f), -100, -75, 740, 555, 20, 40, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, nullptr);
    drawBlendBis(t / 1.454875f * 4 - 0.8f, 1, rgb_a(1, 1, 1, 0.003125f), -100, -75, 740, 555, 20, 50, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, nullptr);
}

void drawCredits(float t)
{
  float sync = t/1.454875f*2-0.5f;
//  float e = exp(6*(rsync-sync));
  texture1->use();

  drawCreditsBack(t);

  //scritte->use();
  glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
  glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);

  //if (avanti)
  //{
    CoolPrint1(*FontArial, 20, sync,  0.0f,  1.0f,  3.0f,  4.0f, 220, sync*30+45, 230, 0.6f, 0.2f, "rio");
    CoolPrint1(*FontArial, 20, sync,  3.0f,  4.0f,  6.0f,  7.0f, 220, sync*30+45, 200, 0.6f, 0.0f, "pan");
    CoolPrint1(*FontArial, 20, sync,  6.0f,  7.0f,  9.0f, 10.0f, 220, sync*30+45, 160, 0.6f, -0.1f, "dixan");
    CoolPrint1(*FontArial, 20, sync,  9.0f, 10.0f, 12.0f, 13.0f, 220, sync*30+45, 170, 0.6f, 0.0f, "wiss");
  //}
  //else
  //{
  //  CoolPrint1(*FontArial, 20, t,  0.0f,  1.0f,  3.0f,  4.0f, 220, t*30+45, 230, 0.6f, 0.2f, "rio");
  //}

  float t2 = panGetTime();
  glClearColor(0,0,0,0.5);

  panViewPerspective();
  //enable stenciling writing
  glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

    //particles
  glDepthMask(GL_FALSE);
  glPushMatrix();
  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glLoadIdentity();
    glRotatef(sinf(t) * 45,0,0,1);
    panViewPerspectiveFOV(45.0f);

    glEnable(GL_TEXTURE_2D);
      glDisable(GL_TEXTURE_GEN_S);
      glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    Vector3 pos(2,-13,-30); //(sin(t) * 3,sin(cos(t * 3)),sin(t) * 3 - 30);
    Vector3 vel(sinf(t2) / 1.5f,fabsf(cosf(t2)),0);
    Vector3 gravity(0.5,5,0);
    Vector3 rot(0,0,0); //sinf(t) * 5.0f, 0.0f , cosf(sinf(t/2.0f) * 1.5f) * 180.0f);
    int tobecreated = 10;
    parts1.SetParticlesPerSec(200);
    parts1.SetSize(0.05f, 0.0f);
    parts1.SetAngle(100.0f);
    parts1.SetAttraction(0);
    parts1.SetLife(3.0f);
    parts1.SetSpeed(3.0f);
    parts1.SetSpread(8.0f,8.0f,2);
    parts1.SetUpdateFlag(UPDATE_AND_CREATE);
    parts1.SetPosition(&pos);
    parts1.SetVelocity(vel);
    parts1.SetGravity(gravity.x,gravity.y,gravity.z);
    parts1.SetSize(0.2f,0.1f);
    parts1.SetRotation(rot.x,rot.y,rot.z);
    parts1.StepOver(t2,tobecreated);
    parts1.Draw(t2);

    pos = Vector3(2,0,-30); //(sin(t) * 3,sin(cos(t * 3)),sin(t) * 3 - 30);
    vel = Vector3(-sinf(t2) / 1.5f,-fabsf(cosf(t2)) * 3,0);
    gravity = Vector3(0.5,-5.0f,0);
    rot = Vector3(0,0,0); //sinf(t) * 5.0f, 0.0f , cosf(sinf(t/2.0f) * 1.5f) * 180.0f);
    tobecreated = 10;
    parts2.SetParticlesPerSec(200);
    parts2.SetSize(0.05f, 0.0f);
    parts2.SetAngle(100.0f);
    parts2.SetAttraction(0);
    parts2.SetLife(3.0f);
    parts2.SetSpeed(3.0f);
    parts2.SetSpread(8.0f,8.0f,2);
    parts2.SetUpdateFlag(UPDATE_AND_CREATE);
    parts2.SetPosition(&pos);
    parts2.SetVelocity(vel);
    parts2.SetGravity(gravity.x,gravity.y,gravity.z);
    parts2.SetSize(0.2f,0.1f);
    parts2.SetRotation(rot.x,rot.y,rot.z);
    parts2.StepOver(t2,tobecreated);
    parts2.Draw(t2);
  glPopMatrix();

  glEnable(GL_CULL_FACE);
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
  glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 0, 0xffffffff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  panViewPerspective();
  glBindTexture(GL_TEXTURE_2D,logo);
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  drawBlendBis(t / 1.45f, 1, rgb_a(1, 1, 1, 0.5), WIDTH / 2, 80, WIDTH, HEIGHT - 80, 10, 50, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, nullptr);

  glDisable(GL_STENCIL_TEST);


  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);

}


#define PANLANDSIZE 50
void dPanLandscape(float t, float nstep, float len, float H) {
  float videostep = len/PANLANDSIZE;
  float heights[PANLANDSIZE][PANLANDSIZE];
  float center = -videostep*(PANLANDSIZE-1)/2.0f;
  for(int i = 0; i < PANLANDSIZE; i++) {
    float x = nstep*i;
    for(int j = 0; j < PANLANDSIZE; j++) {
      float y = nstep*j;
      heights[i][j] = vnoise(x, y, t);
    }
  }

  glPushMatrix();
  glTranslatef(center, 0, center);
  glColor4f(1,1,1,0.06f);
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  float deltatex = 1/(PANLANDSIZE-1);
  for(int i = 0; i < PANLANDSIZE-1; i+=2) {
    float x = videostep*i;
    float x1 = i*deltatex;
    float z, z1;
    for(int j = 0; j < PANLANDSIZE-1; j+=2) {
      z = videostep*j;
      z1 = j*deltatex;
/*
      glVertex3f(x, heights[i][j], z);
      glVertex3f(x, heights[i][j+1], z+videostep);
      glVertex3f(x+videostep, heights[i+1][j+1], z+videostep); // a scambiare questo e il prossimo ci si diverte!
      glVertex3f(x+videostep, heights[i+1][j], z);
*/
      glTexCoord2f(heights[i][j],x1);
      glVertex3f(x, heights[i][j]*H, z);
      glTexCoord2f(heights[i+2][j],x1+deltatex*2);
      glVertex3f(x+videostep*2, heights[i+2][j]*H, z);
      glTexCoord2f(0,x1);
      glVertex3f(x+videostep*2, -20, z);
      glTexCoord2f(0,x1+deltatex*2);
      glVertex3f(x, -20, z);

      glTexCoord2f(heights[i][j],z1);
      glVertex3f(x, heights[i][j]*H, z);
      glTexCoord2f(heights[i][j+2],z1+deltatex*2);
      glVertex3f(x, heights[i][j+2]*H, z+videostep*2);
      glTexCoord2f(0,z1);
      glVertex3f(x, -20, z+videostep*2);
      glTexCoord2f(0,z1+deltatex*2);
      glVertex3f(x, -20, z);
    }
//    z = videostep*(PANLANDSIZE-1);
  }
  glEnd();
  glColor4f(1,1,1,0.2f);

  glDisable(GL_TEXTURE_2D);
  glBegin(GL_LINES);
  for(int i = 0; i < PANLANDSIZE-1; i++) {
    float x = videostep*i;
    float z;
    for(int j = 0; j < PANLANDSIZE-1; j++) {
      z = videostep*j;
      
      glVertex3f(x, heights[i][j]*H, z);
      glVertex3f(x+videostep, heights[i+1][j]*H, z);
      glVertex3f(x, heights[i][j]*H, z);
      glVertex3f(x, heights[i][j+1]*H, z+videostep);
    }
    z = videostep*(PANLANDSIZE-1);
    glVertex3f(x, heights[i][PANLANDSIZE-1]*H, z);
    glVertex3f(x+videostep, heights[i+1][PANLANDSIZE-1]*H, z);
    glVertex3f(z, heights[PANLANDSIZE-1][i]*H, x);              // e' i, non j... coglione! x e z si scambiano...
    glVertex3f(z, heights[PANLANDSIZE-1][i+1]*H, x+videostep);
  }
  glEnd();
  glPopMatrix();
}

void drawPanLandscape(float t) {
	static float fogColor[4] = { 0, 0, 0, 0 };
  panViewPerspective();

  float len = 50;
  glBlendFunc(GL_SRC_ALPHA,GL_ONE);
  glEnable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColor);
//	glFogf(GL_FOG_DENSITY, 0.01f ); 
	glFogf(GL_FOG_START, len/8);//len/4);
	glFogf(GL_FOG_END, len/4);
  glEnable(GL_FOG);
	glHint(GL_FOG_HINT, GL_FASTEST);
  glEnable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHTING);
  glRotatef(t*10, 0, 1, 0);
  glRotatef(15, 1, 0, 0);
  glTranslatef(0, -3, 0);
  dPanLandscape(t, 0.2f, len/2, 1.5f);
  glTranslatef(0, -0.1f, 0);
  dPanLandscape(t+0.2f, 0.2f, len/2, 1.6f);
  glDisable(GL_FOG);
}

void drawPanOverWiss(float t) {
  float t1 = t/1.454875f;

  int stock1 = (int)floorf(t1);
  int stock2 = (int)floorf(t1*8);
  float randa[10];
  float t2 = HALFPI*t1;
  static const float wei[10] = {0, 0, 0, 0, 0, 0, 0, 0.5, 0.5, 0.0}; // da vicino nessuno e' normale! 
  for(int i = 0; i < 10; i++) {
    randa[i] = wei[i]*vlattice(stock2,i)+(1-wei[i])*vlattice(stock1, i+50);
  }
  Vector3 pos(randa[0]*4,randa[1]*4,-18+randa[2]*4);
  Vector3 rot(t * 30+randa[3]*6,sinf(t2) * 30,0);
  drawWissEffect(pos,rot,rgb_a(1.0f, 1.0f, 1.0f, 0.8f),rgb_a(0.3f, 0.2f, 0.1f, fabsf(sinf(t2+randa[8]*3)) / 4.0f),true);
  pos = Vector3(randa[4]*4,randa[5]*4,-12+randa[6]*4);
  rot = Vector3(sinf(t2) * 30, t * 30+randa[7]*6, 0);
  drawWissEffect(pos,rot,rgb_a(1.0f, 1.0f, 1.0f, 0.8f),rgb_a(0.3f, 0.2f, 0.1f, fabsf(sinf(t2+randa[9]*3)) / 4.0f),true);
}

/*##########################################################*/
/*Implementation											*/
/*##########################################################*/

void ScenaRewind(int orderr)
{
float speed = 8.0f;
float mytime = (panGetTime() - timebase[15]) * speed;
float tr;
float durata;

  

  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  durata = (timebase[15 - orderr] - timebase[14 - orderr]);
  tr = (panGetTime() - timebase[15] - (1.4548f * orderr)) / 1.4548f * durata;
  float tr2 = (orderr+1)*1.4548f*speed-mytime;

  CoolPrint1(*FontArial, 50, tr, 0, 1.f, 5.5f, 6.5f, 320, 400, 50-15*(1-1.f/(1.1f+sinf(tr*0.8f))), 0.6f, 0, "from a close sight nobody is normal");
  
  panViewPerspectiveFOV(45);
  drawLines(tr,0.5,30);
  glDisable(GL_FOG);

  //coso energetico
  if (orderr == 0)
  {

    float val = 1;
    if ( tr < 1.4548f)
      val = 7;
    else if ( tr < (1.4548f * 2))
      val = 1;
    else if ( tr < (1.4548f * 3))
      val = 6;
    else if ( tr < (1.4548f * 4))
      val = 2;
    else if ( tr < (1.4548f * 5))
      val = 8;
    else if ( tr < (1.4548f * 6))
      val = 1;
    else if ( tr < (1.4548f * 7)) // domanda: a che (cazzo) serve ?
      val = 7;                   // a farci belli agli occhi di dio
    else if ( tr < (1.4548f * 8)) // ...e io ti piscio in culo...
      val = 1;

    glDisable(GL_FOG);
    PrepareRenderToTexture((100.0f - (sinf(mytime) * 45.0f)), 256);
    drawHelix(mytime * 300,2.0f, 3.5f, (int) val, 30,GL_FILL,rgb_a(1,0.7f,0,0),rgb_a(1,0.7f,0,0.9f));
    drawHelix(mytime * 300,2.0f, 3.5f, (int) val, 30,GL_LINE,rgb_a(1,0.6f,0,0),rgb_a(1,0.6f,0,0.9f));
    DoRenderToTexture(256,frame2);
    donerendertotexture = false;

    drawLines(mytime,0.4f,20);
    panViewPerspective();

    drawTexture(frame2,1,1,true,0.3f,12,rgb_a(1,1,1,0.6f),true);
    drawTexture(frame2,1,1,true,0.4f,15,rgb_a(1,1,1,0.6f),true);
    drawTexture(frame2,1,1,true,0.5f,18,rgb_a(1,1,1,0.6f),true);
    drawTexture(frame2,1,1,true,0.6f,21,rgb_a(0.8f,0.8f,0.8f,0.6f),true);
    drawTexture(frame2,1,1,true,0.7f,24,rgb_a(1,1,1,0.6f),true);
    drawTexture(frame2,1,1,true,0.8f,27,rgb_a(0.7f,0.6f,0.6f,0.6f),true);
    drawTexture(frame2,1,1,true,0.9f,30,rgb_a(1,0.8f,0.8f,0.6f),true);
    drawTexture(frame2,1,1,true,1.0f,33,rgb_a(0.5f,0.4f,0.3f,0.6f),true);

    drawTexture(frame2,1,1,true,1.0f,133,rgb_a(0.5f,0.4f,0.3f,0.6f),true);
    panViewOrtho();

  }

  //peli
  if (orderr == 1)
  {
    
    //scena 13 originale
    glDisable(GL_FOG);
    PrepareRenderToTexture(45, 256); //(100.0f - (sin(mytime) * 45.0f))
    glTranslatef(0,0,-10);
    glRotatef(-sinf(mytime / 10) * 180.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(-sinf(mytime / 10) * 45.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(-sinf(mytime / 10) * 45.0f, 0.0f, 0.0f, 1.0f);
    dCylinder(0.2f,24,12,5,sinf(mytime),cosf(mytime * 2) * 2,sinf(cosf(mytime)),mytime,true,rgb_a(0.9f, 0.9f, 0.9f, 0.0f),rgb_a(1.0f,1.0f,1.0f,0.7f),15);
    DoRenderToTexture(256,frame2);
    donerendertotexture = false;

    panViewPerspective();
    //SUKA MODIFICATE LE SEGUENTI CHIAMATE
    drawTexture(tex,1,1,mytime,mytime / 2,12,rgb_a(0.7f,0.7f,0.7f,0.8f),false);
    
    drawTexture(frame2,1,1,true,0.5f,12,rgb_a(1,1,1,0.8f),true);
    drawTexture(frame2,1,1,true,0.6f,15,rgb_a(1,1,1,0.8f),true);
    drawTexture(frame2,1,1,true,0.7f,18,rgb_a(1,1,1,0.8f),true);
    drawTexture(frame2,1,1,true,0.8f,21,rgb_a(1,1,1,0.8f),true);
    drawTexture(frame2,1,1,true,0.9f,24,rgb_a(1,1,1,0.8f),true);
    drawTexture(frame2,1,1,true,1.0f,27,rgb_a(1,1,1,0.8f),true);
    drawTexture(frame2,1,1,true,1.0f,30,rgb_a(1,1,1,0.8f),true);
    drawTexture(frame2,1,1,true,1.0f,33,rgb_a(1,1,1,0.8f),true);
    //SUKA FINE MODIFICHE

  }

  //pan terreno
  if (orderr == 2)
  {

    //glClearColor(0.0f,0.0f,0.0f,0.0f);
    drawPanLandscape(timebase[11] - tr);
  }

  //credits
  if (orderr == 3)
  {
    glDisable(GL_DEPTH_TEST);
    drawCredits(tr2);
  }

  //roba wiss
  if (orderr == 4)
  {
    drawLines(tr,0.1f, 50);
    float t1 = tr/1.454875f;

    int stock2 = (int) floorf(t1*8);

    if (stock2 < 52) {
      drawPanOverWiss(tr);
      panViewOrtho();
      scritte->use();
	    glEnable(GL_TEXTURE_GEN_S);
	    glEnable(GL_TEXTURE_GEN_T);
      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
//      CoolPrint1(*FontArial, 50, tr, 0, 1.f, 5.5f, 6.5f, 320, 400, 50-15*(1-1.f/(1.1f+sin(tr*0.8))), 0.6f, 0, "from a close sight nobody is normal");
	    glDisable(GL_TEXTURE_GEN_S);
	    glDisable(GL_TEXTURE_GEN_T);
      glDisable(GL_TEXTURE_2D);
    } else {
      float t2 = HALFPI*t1;
      Vector3 pos(0, 0,-18+(stock2-52));
      Vector3 rot(tr * 30,sinf(t2) * 30,0);
      drawWissEffect(pos,rot,rgb_a(1.0f, 1.0f, 1.0f, 0.8f),rgb_a(0.3f, 0.2f, 0.1f, fabsf(sinf(t2*5)) / 4.0f),true);
      glColor4f(1,0.9f,0.8f,(stock2-52)*0.125f);
      panViewOrtho();
      glBegin(GL_QUADS);
      glVertex2f(0, 0);
      glVertex2f(WIDTH, 0);
      glVertex2f(WIDTH, HEIGHT);
      glVertex2f(0, HEIGHT);
      glEnd();
    }
  }

  //go
  if (orderr == 5) // ma si, anche senzuncazzo sta bene...
  {
  }

  //fade
  if (orderr == 6)
  {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
//    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
//    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
//    tex->use();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);

    glBlendFunc(GL_SRC_ALPHA,GL_SRC_ALPHA);
    glEnable(GL_BLEND);
    panViewOrtho();
    scritte->use();
	  glEnable(GL_TEXTURE_GEN_S);
	  glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
//    CoolPrint1(*FontArial, 50, tr, 0, 0.1f, 5.5f, 6.5f, WIDTH / 2, HEIGHT / 2 - 20, 200+50*sin(tr), 0.5f, -0.25*sin(tr), "GO");
    glColor3f(1, 1, 1);
	  glDisable(GL_TEXTURE_GEN_S);
	  glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
  }

  //tubo peloso
  if (orderr == 7)
  {
    panViewPerspectiveFOV(45);
    drawTubo(6,tr,rgb_a(0.9f, 0.9f, 0.9f, 0.1f),rgb_a(0.5,0.25,0.0625,0.5f));
  }
  
  //palline
  if (orderr == 8)
  {
    drawTexture(frame,1.0f,1.0f,true,tr,80,rgb_a(1,1,1,0.6f),true);
    drawTexture(frame,1.0f,1.0f,false,tr,80,rgb_a(1,1,1,0.6f),true);
    drawNuts(tr);
    drawWissEffect(Vector3(3, 3, -4),Vector3(mytime, mytime, 0),rgb_a(1.0f, 1.0f, 1.0f, 0.8f),rgb_a(0.3f, 0.2f, 0.1f, fabsf(sinf(mytime)) / 4.0f),false);
  }

  //tubo peloso
  if (orderr == 9)
  {
    panViewPerspectiveFOV(45);
    drawTubo(10,tr,rgb_a(0.9f, 0.9f, 0.9f, 0.1f),rgb_a(0.5,0.25,0.0625,0.5f));
  }
  
  //palline
  if (orderr == 10)
  {
    drawTexture(frame,1.0f,1.0f,true,tr,80,rgb_a(1,1,1,0.6f),true);
    drawTexture(frame,1.0f,1.0f,false,tr,80,rgb_a(1,1,1,0.6f),true);
    drawNuts(tr);
    drawWissEffect(Vector3(3, 3, -4),Vector3(mytime, mytime, 0),rgb_a(1.0f, 1.0f, 1.0f, 0.8f),rgb_a(0.3f, 0.2f, 0.1f, fabsf(sinf(mytime)) / 4.0f),false);
  }

  //sfondo vuoto + texture toroide
  if (orderr == 11)
  {
    drawTexture(frame,1.0f,1.0f,true,tr,80,rgb_a(1,1,1,0.6f),true);
    drawTexture(frame,1.0f,1.0f,false,tr,80,rgb_a(1,1,1,0.6f),true);
    drawWissEffect(Vector3(3, 3, -4),Vector3(mytime, mytime, 0),rgb_a(1.0f, 1.0f, 1.0f, 0.8f),rgb_a(0.3f, 0.2f, 0.1f, fabsf(sinf(mytime)) / 4.0f),false);
  }

  //flash + toroide
  if (orderr == 12)
  {
    drawSfondo(Vector3(sinf(tr) * 90.0f,0,sinf(tr*2) * 45.0f),rgb_a(0.5,0.5,1.0,1));
    glDisable(GL_CULL_FACE);
    panViewPerspectiveFOV(190.0f - ((tr / durata) * 70.0f));
    drawToroide(orderr,tr,mytime);
    float val = sinf(mytime)*2.5f;
    glPushMatrix();
    glTranslatef(val, 0, 0);
    drawToroide(orderr,tr+50,mytime+1);
    glTranslatef(-2*val, 0, 0);
    drawToroide(orderr,tr+50,mytime+2);
    glPopMatrix();
  }

  //toroide
  if (orderr >= 13)
  {
    drawSfondo(Vector3(sinf(tr) * 90.0f,0,sinf(tr*2) * 45.0f),rgb_a(0.5,0.5,1.0,1));
    glDisable(GL_CULL_FACE);
    panViewPerspectiveFOV(110.0f - ((tr / durata) * 70.0f));
    drawToroide(orderr,tr,mytime);
    float val = sinf(mytime)*2.5f;
    glPushMatrix();
    glTranslatef(val, 0, 0);
    drawToroide(orderr,tr+50,mytime+1);
    glTranslatef(-2*val, 0, 0);
    drawToroide(orderr,tr+50,mytime+2);
    glPopMatrix();
  }
}



void Scena(float t, int order) {

  mytime = panGetTime();
  if (!isMusicEnabled)
    skTimerFrame();

  //static GLTexture* frame =  perlin(8, 132, 0.3f, 0.7f, true);

//################### rIO stuff do not even try to understand that  :D
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
#ifndef caccaculo
  tex2->use();
  //merdaccia per renderare nella texture i resti del toroide
  //SHIT SHIT SHIT!!! used to render once in the texture the rest of first scene torus
  if ((order == 2) && (donerendertotexture == false))
  { 
      PrepareRenderToTexture((180.0f - (mytime * 15.0f)), 256);
    //BEWARE oof matrices, do not use other drawing calls, or pay attention to
    //matrices they set, in this part no matrices ops should be done!!
    //attenzione alla merda che fanno le altre scene NON USARLE PER ORA!
    //drawSfondo(order,t);
//      panViewPerspectiveFOV(180.0f - (panGetTime() * 15.0f));
      drawToroide(order,t,mytime);
      float val = sinf(mytime)*2.5f;
      glPushMatrix();
      glTranslatef(val, 0, 0);
      drawToroide(order,t+50,mytime+1);
      glTranslatef(-2*val, 0, 0);
      drawToroide(order,t+50,mytime+2);
      glPopMatrix();
    DoRenderToTexture(256,frame);
  }
    
  if ((order == 7) && (donerendertotexture == false))
  {
    PrepareRenderToTexture(45, 256);
    //BEWARE oof matrices, do not use other drawing calls, or pay attention to
    //matrices they set, in this part no matrices ops should be done!!
    //attenzione alla merda che fanno le altre scene NON USARLE PER ORA!
    //drawSfondo(order,t);
    drawTubo(order,timebase[7] - timebase[6],rgb_a(0.9f, 0.9f, 0.9f, 0.1f),rgb_a(0.5,0.25,0.0625,0.5f));
    DoRenderToTexture(256,frame2);
  }

  if (order == 0)
    drawSfondo(Vector3(sinf(t) * 90.0f,0,sinf(t*2) * 45.0f),rgb_a(0.5,0.5,1.0,1));
  
  if (order == 1)
  {
    glDisable(GL_TEXTURE_2D);
    drawBlend(rgb_a(1, 1, 1, 1.0f - (t / 2)), 0, 0, WIDTH, HEIGHT, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, nullptr);
  }

  if (order < 4)  //(order == 0) && 
  {
      panViewPerspectiveFOV(180.0f - (panGetTime() * 15.0f));
      drawToroide(order,t,mytime);
      float val = sinf(mytime)*2.5f;
      glPushMatrix();
      glTranslatef(val, 0, 0);
      drawToroide(order,t+50,mytime+1);
      glTranslatef(-2*val, 0, 0);
      drawToroide(order,t+50,mytime+2);
      glPopMatrix();

      panViewOrtho();
      
      if (order == 0) {
	  glDisable(GL_CULL_FACE);
	  scritte->use();
      glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
      glEnable(GL_BLEND);
	      glEnable(GL_TEXTURE_GEN_S);
	      glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	CoolPrint1(*FontArial, 50,    t, 0.0f, 1.5f, 8.0f, 10.0f, 240, 140, 60, 0.6f, -0.2f, "reality is that which");
        CoolPrint1(*FontArial, 50,    t, 1.5f, 3.0f, 8.0f, 10.0f, 400, 340, 60, 0.6f, -0.2f, "doesn't go away");
        CoolPrint1(*FontArial, 50, 10-t, -1.0f, 1.0f, 5.0f, 7.0f, 320, 240, 40, 0.6f, 0.2f, "when you stop believing in it");
//        CoolPrint1(*FontArial, 50,    t, 6.0f, 8.0f, 9.0f, 10.0f, 480, 400, 60, 0.6f, -0.2f, "Philip K. Dick");
	      glDisable(GL_TEXTURE_GEN_S);
	      glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_2D);
      }
  }


  if (order == 2) // && ((0.4f - (mytime / 100.0f)) > 0))
  {
      panViewPerspectiveFOV(180.0f - (mytime * 15.0f));
      drawToroide(order,t, mytime);
      drawTexture(frame,1.0f,1.0f,true,t,80,rgb_a(1,1,1,fabsf(sinf(t * 1.3f))),true);
      drawTexture(frame,1.1f,1.0f,false,t,80,rgb_a(1,1,1,fabsf(sinf(t * 1.2f))),true);
      drawTexture(frame,1.2f,1.0f,false,t,80,rgb_a(1,1,1,fabsf(sinf(t * 1.1f))),true);
      drawTexture(frame,1.3f,1.0f,false,t,80,rgb_a(1,1,1,fabsf(sinf(t * 1.0f))),true);
      drawWissEffect(Vector3(3, 3, -4),Vector3(mytime, mytime, 0),rgb_a(1.0f, 1.0f, 1.0f, 0.8f),rgb_a(0.3f, 0.2f, 0.1f, fabsf(sinf(mytime)) / 4.0f),false);

      glBlendFunc(GL_SRC_ALPHA,GL_SRC_ALPHA);
      glEnable(GL_BLEND);
      panViewOrtho();
      scritte->use();
	    glEnable(GL_TEXTURE_GEN_S);
	    glEnable(GL_TEXTURE_GEN_T);
      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
      CoolPrint1(*FontArial, 50, t, 0, 1.5f, 1.5f, 3.0f, 140, 80, 60, 0.6f, 0.5f, "clean");
      CoolPrint1(*FontArial, 50, t, 1.5f, 3.0f, 3.0f, 4.5f, 500, 400, 40, 0.8f, -0.1f, "perceptions");
      glColor3f(1, 1, 1);
	    glDisable(GL_TEXTURE_GEN_S);
	    glDisable(GL_TEXTURE_GEN_T);
      glDisable(GL_TEXTURE_2D);
      drawLines(t,0.3f, 10);
  }

  if ((order == 3) || (order == 5))
  {
    drawTexture(frame,1.0f,1.0f,true,t,80,rgb_a(1,1,1,0.6f),true);
    drawTexture(frame,1.0f,1.0f,false,t,80,rgb_a(1,1,1,0.6f),true);
    drawNuts(t);
    drawWissEffect(Vector3(3, 3, -4),Vector3(mytime, mytime, 0),rgb_a(1.0f, 1.0f, 1.0f, 0.8f),rgb_a(0.3f, 0.2f, 0.1f, fabsf(sinf(mytime)) / 4.0f),false);

    glBlendFunc(GL_SRC_ALPHA,GL_SRC_ALPHA);
    glEnable(GL_BLEND);
    panViewOrtho();
    scritte->use();
	  glEnable(GL_TEXTURE_GEN_S);
	  glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    if (order == 3) {
      CoolPrint1(*FontArial, 50, t, -1.5f, 0.0f, 1.0f, 2.5f, 500, 400, 40, 0.8f, -0.1f, "perceptions");
    } else {
      CoolPrint1(*FontArial, 50, t, 0, 1.5f, 1.5f, 3.0f, 140, 80, 60, 0.6f, 0.5, "clear");
      CoolPrint1(*FontArial, 50, t, 1.5f, 3.0f, 3.0f, 4.5f, 500, 400, 40, 0.8f, -0.1f, "illusions");
    }
    glColor3f(1, 1, 1);
	  glDisable(GL_TEXTURE_GEN_S);
	  glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
    drawLines(t,0.3f, 10);
  }

  if ((order == 4) || (order == 6))
  {
    drawSfondo(Vector3(sinf(t) * 90.0f,0,sinf(t*2) * 45.0f),rgb_a(0.5,0.5,1.0,1));
    panViewPerspectiveFOV(45);
//    drawTubo(order,t);
//    drawTubo(order,mytime,rgb_a(0.9f, 0.9f, 0.9f, 0.1f),rgb_a(0.5,0.25,0.0625,0.5f));
    drawTubo(order,t,rgb_a(0.9f, 0.9f, 0.9f, 0.1f),rgb_a(0.5,0.25,0.0625,0.5f));
    if (donerendertotexture)
      donerendertotexture = false;

    glBlendFunc(GL_SRC_ALPHA,GL_SRC_ALPHA);
    glEnable(GL_BLEND);
    panViewOrtho();
    scritte->use();
	  glEnable(GL_TEXTURE_GEN_S);
	  glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    if (order == 4) {
      CoolPrint1(*FontArial, 50, t, 0, 1.5f, 1.5f, 3.0f, 140, 80, 60, 0.6f, 0.5, "fuzzy");
      CoolPrint1(*FontArial, 50, t, 1.5f, 3.0f, 3.0f, 4.5f, 500, 400, 40, 0.8f, -0.1f, "dimensions");
    } else {
      CoolPrint1(*FontArial, 50, t, 0, 1.5f, 1.5f, 3.0f, 140, 80, 60, 0.6f, 0.5, "furry");
      CoolPrint1(*FontArial, 50, t, 1.5f, 3.0f, 3.0f, 4.5f, 500, 400, 40, 0.8f, -0.1f, "onions");
    }
    glColor3f(1, 1, 1);
	  glDisable(GL_TEXTURE_GEN_S);
	  glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
  }

  if (order == 7)
  {
    if ((t * 1.6f) < 1.0f)
    {
      panViewPerspectiveFOV(45);
      drawTubo(order,mytime,rgb_a(0.9f, 0.9f, 0.9f, 0.1f - (t * 2)),rgb_a(0.5f - (t * 2),0.25f - (t * 2),0.0625f - (t * 2),0.5f - (t * 4)));

      drawBlend(rgb_a(1,1,1,1 - (t * 2.0f)),0,(int) -(t * 150.6f),WIDTH,(int) (HEIGHT + (t * 150.6f)),GL_SRC_ALPHA,GL_ONE,frame2);
      drawBlend(rgb_a(1,1,1,1 - (t * 2.0f)),0,(int) -(t * 150.6f),WIDTH,(int) (HEIGHT + (t * 150.6f)),GL_SRC_ALPHA,GL_ONE,frame2);
      drawBlend(rgb_a(1,1,1,1 - (t * 2.0f)),0,(int) -(t * 150.6f),WIDTH,(int) (HEIGHT + (t * 150.6f)),GL_SRC_ALPHA,GL_ONE,frame2);
    }
  }

  if (order == 8)
  {
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
//    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
//    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
//    tex->use();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);

    glColor4f(1.0f,1.0f,1.0f,0.6f);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glEnable(GL_BLEND);
    panViewOrtho();
    scritte->use();
	  glEnable(GL_TEXTURE_GEN_S);
	  glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    CoolPrint1(*FontArial, 50, t, 0, 0.1f, 5.5f, 6.5f, WIDTH / 2, HEIGHT / 2 - 20, 200+50*sinf(t), 0.5f, -0.25f*sinf(t), "GO");
    CoolPrint1(*FontArial, 50, t, 0, 0.1f, 5.5f, 6.5f, 500, 400, 50, 1.5f, -0.25f*sinf(t), "where?");
    glColor3f(1, 1, 1);
	  glDisable(GL_TEXTURE_GEN_S);
	  glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
  }

  if (order == 9)
  {
    drawLines(t,0.1f, 50);
    float t1 = t/1.454875f;

    int stock2 = (int) floorf(t1*8);

    if (stock2 < 52) {
      drawPanOverWiss(t);
      panViewOrtho();
      scritte->use();
	    glEnable(GL_TEXTURE_GEN_S);
	    glEnable(GL_TEXTURE_GEN_T);
      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
      CoolPrint1(*FontArial, 50, t, 0, 1.f, 5.5f, 6.5f, 320, 400, 50 - 15 * (1 - 1.f / (1.1f + sinf(t * 0.8f))), 0.6f, 0, "from a close sight nobody is normal");
	    glDisable(GL_TEXTURE_GEN_S);
	    glDisable(GL_TEXTURE_GEN_T);
      glDisable(GL_TEXTURE_2D);
    } else {
      float t2 = HALFPI*t1;
      Vector3 pos(0, 0,-18+(stock2-52));
      Vector3 rot(t * 30,sinf(t2) * 30,0);
      drawWissEffect(pos,rot,rgb_a(1.0f, 1.0f, 1.0f, 0.8f),rgb_a(0.3f, 0.2f, 0.1f, fabsf(sinf(t2*5)) / 4.0f),true);
      glColor4f(1,0.9f,0.8f,(stock2-52)*0.125f);
      panViewOrtho();
      glBegin(GL_QUADS);
      glVertex2f(0, 0);
      glVertex2f(WIDTH, 0);
      glVertex2f(WIDTH, HEIGHT);
      glVertex2f(0, HEIGHT);
      glEnd();
    }

  }

  if (order == 10)

    drawCredits(t);

  if (order == 11)
  {
    //glClearColor(0.0f,0.0f,0.0f,0.0f);
    drawPanLandscape(mytime);
    glDisable(GL_FOG);
    panViewOrtho();
    scritte->use();
	  glEnable(GL_TEXTURE_GEN_S);
	  glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    CoolPrint1(*FontArial, 50, t, 1.0, 2.0f, 5.8f, 7.0f, WIDTH / 2, 80, 100+25*sinf(t), 0.7f, -0.25f*sinf(t), "look around");
    CoolPrint1(*FontArial, 50, t, 5.8f, 7.0f, 10.6f, 11.6f, WIDTH / 2, 80, 100+25*sinf(t), 0.7f, -0.25f*sinf(t), "look inside");
    glColor3f(1, 1, 1);
	  glDisable(GL_TEXTURE_GEN_S);
	  glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
  }

  //SUKA MODIFICATA ANCHE QUESTA PARTE (ORA RICHIMA LA DRAW TEXTURE USANDO IL TWIRL)
  //peli
  if (order == 12)
  {
    glDisable(GL_FOG);
    PrepareRenderToTexture(45, 256); //(100.0f - (sin(mytime) * 45.0f))
    glTranslatef(0,0,-10);
    glRotatef(sinf(mytime / 10) * 180.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(sinf(mytime / 10) * 45.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(sinf(mytime / 10) * 45.0f, 0.0f, 0.0f, 1.0f);
    dCylinder(0.2f,24,12,5,sinf(mytime),cosf(mytime * 2) * 2,sinf(cosf(mytime)),mytime,true,rgb_a(0.9f, 0.9f, 0.9f, 0.0f),rgb_a(1.0,1.0,1.0,0.7f),15);
    DoRenderToTexture(256,frame2);
    donerendertotexture = false;

    panViewPerspective();
    //SUKA MODIFICATE LE SEGUENTI CHIAMATE
    drawTexture(tex,1,1,mytime,mytime / 2,12,rgb_a(0.7f,0.7f,0.7f,0.8f),false);
    

    drawTexture(frame2,1,1,true,0.5f,12,rgb_a(1,1,1,0.8f),true);
    drawTexture(frame2,1,1,true,0.6f,15,rgb_a(1,1,1,0.8f),true);
    drawTexture(frame2,1,1,true,0.7f,18,rgb_a(1,1,1,0.8f),true);
    drawTexture(frame2,1,1,true,0.8f,21,rgb_a(1,1,1,0.8f),true);
    drawTexture(frame2,1,1,true,0.9f,24,rgb_a(1,1,1,0.8f),true);
    drawTexture(frame2,1,1,true,1.0f,27,rgb_a(1,1,1,0.8f),true);
    drawTexture(frame2,1,1,true,1.0f,30,rgb_a(1,1,1,0.8f),true);
    drawTexture(frame2,1,1,true,1.0f,33,rgb_a(1,1,1,0.8f),true);
    //SUKA FINE MODIFICHE

    glBlendFunc(GL_SRC_ALPHA,GL_SRC_ALPHA);
    glEnable(GL_BLEND);
    panViewOrtho();
    scritte->use();
	  glEnable(GL_TEXTURE_GEN_S);
	  glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    CoolPrint1(*FontArial, 50, t, 0, 1.0f, 11.4f, 11.6f, 320, 60, 50+5*sinf(t), 0.5f, 0, "do you have the");
    CoolPrint1(*FontArial, 50, t, 1.0f, 2.0f, 11.4f, 11.6f, 320, 400, 120, 1.0f, -0.25f*sinf(t), "POWER");
  }

  //flusso energetico lamadonna
  if (order == 13)
  {
    float val = 1;
    if ( t < 1.4548f)
      val = 7;
    else if ( t < (1.4548f * 2))
      val = 1;
    else if ( t < (1.4548f * 3))
      val = 6;
    else if ( t < (1.4548f * 4))
      val = 2;
    else if ( t < (1.4548f * 5))
      val = 8;
    else if ( t < (1.4548f * 6))
      val = 1;
    else if ( t < (1.4548f * 7)) 
      val = 7;
    else if ( t < (1.4548f * 8))
      val = 1;
/*      
    glDisable(GL_FOG);
    PrepareRenderToTexture(45.0f, 256);
    drawHelix(mytime * -300,2.0f, 3.5f, val, 30,GL_FILL,rgb_a(1,0.7,0.0,1.0),rgb_a(0.5,0.5,0.5,0.5));
    DoRenderToTexture(256,frame2);
    donerendertotexture = false;
*/
    glDisable(GL_FOG);
    PrepareRenderToTexture((100.0f - (sinf(mytime) * 45.0f)), 256);
    drawHelix(mytime * -300,2.0f, 3.5f, (int) val, 30,GL_FILL,rgb_a(1,0.7f,0,0),rgb_a(1,0.7f,0,0.9f));
    drawHelix(mytime * -300,2.0f, 3.5f, (int) val, 30,GL_LINE,rgb_a(1,0.6f,0,0),rgb_a(1,0.6f,0,0.9f));
    DoRenderToTexture(256,frame2);
    donerendertotexture = false;

    drawLines(mytime,0.4f,20);
    panViewPerspective();

    drawTexture(frame2,1,1,true,0.3f,12,rgb_a(1,1,1,0.6f),true);
    drawTexture(frame2,1,1,true,0.4f,15,rgb_a(1,1,1,0.6f),true);
    drawTexture(frame2,1,1,true,0.5f,18,rgb_a(1,1,1,0.6f),true);
    drawTexture(frame2,1,1,true,0.6f,21,rgb_a(0.8f,0.8f,0.8f,0.6f),true);
    drawTexture(frame2,1,1,true,0.7f,24,rgb_a(1,1,1,0.6f),true);
    drawTexture(frame2,1,1,true,0.8f,27,rgb_a(0.7f,0.6f,0.6f,0.6f),true);
    drawTexture(frame2,1,1,true,0.9f,30,rgb_a(1,0.8f,0.8f,0.6f),true);
    drawTexture(frame2,1,1,true,1.0f,33,rgb_a(0.5f,0.4f,0.3f,0.6f),true);

    drawTexture(frame2,1,1,true,1.0f,133,rgb_a(0.5f,0.4f,0.3f,0.6f),true);
    panViewOrtho();

    scritte->use();
	  glEnable(GL_TEXTURE_GEN_S);
	  glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    CoolPrint1(*FontArial, 50, t, 0, 1.0f, 5.6f, 5.8f, 320, 100, 50+5*sinf(t), 0.5f, 0, "to make your  d r e a m s ");
    CoolPrint1(*FontArial, 50, t, 5.7f, 5.9f, 11.4f, 11.6f, 320, 100, 50+5*sinf(t), 0.5f, 0, "to make your nightmares");
    CoolPrint1(*FontArial, 50, t, 1.0f, 2.0f, 11.4f, 11.6f, 320, 300, 120, 1.0f, -0.25f*sinf(t), "REAL?");
    glColor3f(1, 1, 1);
	  glDisable(GL_TEXTURE_GEN_S);
	  glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);

  }


  //SUKA ULTIMA SCENA DEI CAZZIBBUFI CHE CAMMINANO
  //bugs
  if (order == 14)
  {
    Vector3 occhio;
    if (t < (1.4548f * 4))
      occhio = Vector3(0,0,0);
    else if ( (t >= (1.4548f * 4)) && (t < (1.4548f * 5)) )
    {
      float ang = ((t - (1.4548f * 4)) / 1.4548f) * PI;
      occhio = Vector3(-sinf(ang) * 15 ,(t - (1.4548f * 4)) * 4, -(t - (1.4548f * 4))*15 );
    }
    else if (t >= (1.4548f * 5))
    {
      occhio = Vector3(0 , 1.4548f * 4 , -1.4548f*15 );
    }

    glEnable(GL_LIGHT0);
    drawBugs(mytime - timebase[14],rgb_a(1,1,1,1),Vector3(0,-1,-8),Vector3(15,15,0),Vector3(6,1,30), - (t / 2.8f),occhio);

    glDisable(GL_LIGHT0);
    glDisable(GL_FOG);

    panViewOrtho();

    scritte->use();
    glBlendFunc(GL_SRC_ALPHA,GL_SRC_ALPHA);
    glEnable(GL_BLEND);
	  glEnable(GL_TEXTURE_GEN_S);
	  glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

    CoolPrint1(*FontArial, 50, t, 0, 1, 10.5f, 11.5f, 370, 120, 70, 0.7f, 0, "Join us!");
    glColor3f(1, 1, 1);
	  glDisable(GL_TEXTURE_GEN_S);
	  glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
  
    drawLines(t,0.3f,20);
  }

  if (order == 15)
#endif
  {
    glClearColor(0.0f,0.0f,0.0f,0.5f);
    float sync = t/1.454875f*2-0.5f;
    float rsync = (sync>0)?floorf(sync):-0.5f;
//    float e = exp(6*(rsync-sync));
    ScenaRewind((int)rsync);
    //flash
    panViewOrtho();

    scritte->use();
    glBlendFunc(GL_SRC_ALPHA,GL_SRC_ALPHA);
    glEnable(GL_BLEND);
	  glEnable(GL_TEXTURE_GEN_S);
	  glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

    static const char * nomi[12] = {"haujobb", "", "mfx", "farbrausch","mewlers","vantage","foobug","purple","kolor","hirmu","calodox"};
// quello vuoto e' vuoto perche' non si vede
    for(int i = 0; i < 12; i++) {
      CoolPrint1(*FontArial, 50, sync, i-0.1f, i, i+1, i+1.1f, 420-160*(i%2), i*30+100, 110, 0.7f, 0, nomi[i]);
    }


    glColor3f(1, 1, 1);
	  glDisable(GL_TEXTURE_GEN_S);
	  glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
  
  }
  
  
  //################### PaN stuff 

  if ((order == 9) || (order == 10) || (order == 15) || (order == 12) || (order == 13)) 
  {
    //flash
    float sync = t/1.454875f*2-0.5f;
    float rsync = (sync>0)?floorf(sync):-0.5f;
    float e = expf(6*(rsync-sync));

    if ((order != 15) || (rsync <= 13)) {
      glClearColor(0,0,0,0.5);
      texture2->use();
      drawBlendBis(rsync, 1, rgb_a(4.8f, 4.5f, 4.5f, 1.33f * e), -100, -75, 740, 555, 20, 50, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, nullptr);
      drawBlendBis(rsync, 1, rgb_a(4.5f, 4.8f, 4.5f, 1.33f * e), -100, -75, 740, 555, 20, 50, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, nullptr);
      drawBlendBis(rsync, 1, rgb_a(4.5f, 4.5f, 4.8f, 1.33f * e), -100, -75, 740, 555, 20, 50, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, nullptr);
    }
    if (order == 9) { // solo sui wisscosi!
      if (sync > 13) { // total in sync 14.4, in time 10.471
        glColor4f(0,0,0, (t-12.5f)/1.9f); // 10.471
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(WIDTH, 0);
        glVertex2f(WIDTH, HEIGHT);
        glVertex2f(0, HEIGHT);
        glEnd();
      }
    }
  }


  //SUKA scena vuota per PAN fine demo...
  //stoppare la musica stocazzo.
  if (order == 16)
  {
    panViewOrtho();

    scritte->use();
    glBlendFunc(GL_SRC_ALPHA,GL_SRC_ALPHA);
    glEnable(GL_BLEND);
	  glEnable(GL_TEXTURE_GEN_S);
	  glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    CoolPrint1(*FontArial, 50, t, -0.06f, 0.00f, 5.6f, 5.8f, 270, 120, 120, 0.7f, 0, "P");
    CoolPrint1(*FontArial, 50, t,  0.20f, 0.26f, 5.6f, 5.8f, 370, 120, 120, 0.7f, 0, "K");
    CoolPrint1(*FontArial, 50, t,  0.47f, 0.53f, 11.4f, 11.6f, 320, 240, 120, 0.7f, 0, "IS");
    CoolPrint1(*FontArial, 50, t,  0.73f, 0.79f, 11.4f, 11.6f, 320, 360, 120+40*sinf((t-0.73f)*3), 0.7f, 0, "DEAD");
    glColor3f(1, 1, 1);
	  glDisable(GL_TEXTURE_GEN_S);
	  glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
  }
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);//_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

  panViewPerspective();

  //leave it for last (rIO)
    drawBorder(rgb_a(0,0,0,1),rgb_a(0,0,0,0),80);

	glDepthMask(GL_TRUE);

}

#ifdef WIN32
LRESULT CALLBACK skWinProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) 
{

  switch(msg) {	
  case WM_KEYDOWN: 
    if ((int)wp != VK_ESCAPE)
      break;
  case WM_CLOSE: 
  case WM_DESTROY: 
    PostQuitMessage(0);
		return 0;
  } 
  return DefWindowProc(wnd, msg, wp, lp);
} 
#endif /* WIN32 */

void skSwappuffers() 
{
  glFinish();
  glFlush();
#ifdef WIN32
  SwapBuffers(hDC);
#else
  SDL_GL_SwapBuffers();
#endif
}

#ifdef __LINUX__
int done;
#endif /* __LINUX__ */

void skDraw() {
  static int maxscene = (sizeof(timebase)/sizeof(float));
  float t = panGetTime();

//  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glEnable(GL_LIGHT0);

  glEnable(GL_LIGHTING);
  glDisable(GL_CULL_FACE);
	
//  glEnable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);

  panViewOrtho();

  if (hiddenpart == false)
  {
    static int scene = 0;
    //ARATEMP
    //t+=117.785;
    //
    if ((scene < maxscene) && (t > timebase[scene+1]))
      scene++;
    if (scene == maxscene) 
    {
      if (isMusicEnabled)
      {
  	    FMUSIC_FreeSong(fmodule);
  	    FSOUND_Close();
        isMusicEnabled = false;
      }
      static float closetime = skGetTime();
      float curtime = skGetTime()-closetime;
      if (curtime>7){
#ifdef WIN32
          PostQuitMessage(0);
#else
          done = 1;
#endif
          return;
      }
      glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
      glColor4f(1.0f,1.0f,1.0f,0.6f);
      glBlendFunc(GL_SRC_ALPHA,GL_SRC_ALPHA);
      glEnable(GL_BLEND);
//    panViewOrtho();
      scritte->use();
	    glEnable(GL_TEXTURE_GEN_S);
	    glEnable(GL_TEXTURE_GEN_T);
      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
      CoolPrint1(*FontArial, 50, curtime, 0.0f, 1.0f, 6.0f, 7.0f, 320,  50, 50, 0.6f, 0.1f, "MekkaSymposium 2002");
      CoolPrint1(*FontArial, 10, curtime, 0.0f, 1.0f, 6.0f, 7.0f, 320, 300, 20, 0.8f, 0.0f, "this spurious reality hasn't been manufactured by");
      CoolPrint1(*FontArial, 30, curtime, 0.0f, 1.0f, 6.0f, 7.0f, 320, 325, 30, 0.8f, 0.1f, "Runciter Associates");
      CoolPrint1(*FontArial, 10, curtime, 0.0f, 1.0f, 6.0f, 7.0f, 320, 350, 20, 0.8f, 0.0f, "and in any case not in collaboration with");
      CoolPrint1(*FontArial, 100, curtime, 0.0f, 1.0f, 6.0f, 7.0f, 320, 385, 80, 0.8f, 0.1f, "SpinningKids");
      glColor3f(1, 1, 1);
	    glDisable(GL_TEXTURE_GEN_S);
	    glDisable(GL_TEXTURE_GEN_T);
      glDisable(GL_TEXTURE_2D);
      drawLines(curtime,0.1f, 100);
  //    return;
    } else {
      Scena(t-timebase[scene], scene);
    }
  }
  else
  {
      glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
      glBlendFunc(GL_SRC_ALPHA,GL_SRC_ALPHA);
      glEnable(GL_BLEND);
      panViewOrtho();
      scritte->use();
	    glEnable(GL_TEXTURE_GEN_S);
	    glEnable(GL_TEXTURE_GEN_T);
      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BLEND);
      glDisable(GL_LIGHTING);
      glColor3f(1,1,1);      
      CoolPrint1(*FontArial, 20,    t, 0.0f, 1.0f, 6.0f, 7.0f, 160, 20, 40, 0.5f, -0.2f, "P.K.Dick Bibliography :");
      CoolPrint1(*FontArial, 20,    t, 1.0f, 2.0f, 7.0f, 8.0f, 130, 40, 40, 0.4f, -0.2f, "Solar Lottery (1955)");
      CoolPrint1(*FontArial, 20,    t, 2.0f, 3.0f, 8.0f, 9.0f, 200, 60, 40, 0.4f, -0.2f, "The World Jones Made (1956)");
      CoolPrint1(*FontArial, 20,    t, 3.0f, 4.0f, 9.0f, 10.0f, 180, 80, 40, 0.4f, -0.2f, "The Man Who Japed (1956)");
      CoolPrint1(*FontArial, 20,    t, 4.0f, 5.0f, 10.0f, 11.0f, 140, 100, 40, 0.4f, -0.2f, "Eye in the sky (1957)");
      CoolPrint1(*FontArial, 20,    t, 5.0f, 6.0f, 11.0f, 12.0f, 185, 120, 40, 0.4f, -0.2f, "The Cosmic Puppets (1957)");
      CoolPrint1(*FontArial, 20,    t, 6.0f, 7.0f, 12.0f, 13.0f, 160, 140, 40, 0.4f, -0.2f, "Time out of Joint (1959)");

      CoolPrint1(*FontArial, 20,    t, 7.0f, 8.0f, 13.0f, 14.0f, 125, 160, 40, 0.4f, -0.2f, "Dr.Futurity (1960)");
      CoolPrint1(*FontArial, 20,    t, 8.0f, 9.0f, 14.0f, 15.0f, 165, 180, 40, 0.4f, -0.2f, "Vulcan's Hammer (1960)");
      CoolPrint1(*FontArial, 20,    t, 9.0f, 10.0f, 15.0f, 16.0f, 195, 200, 40, 0.4f, -0.2f, "Man in the high castle (1963)");
      CoolPrint1(*FontArial, 20,    t, 10.0f, 11.0f, 16.0f, 17.0f, 200, 220, 40, 0.4f, -0.2f, "Game Players of Titan (1963)");

      CoolPrint1(*FontArial, 20,    t, 11.0f, 12.0f, 17.0f, 18.0f, 185, 240, 40, 0.4f, -0.2f, "Martian Time Sleep (1964)");
      CoolPrint1(*FontArial, 20,    t, 12.0f, 13.0f, 18.0f, 19.0f, 140, 260, 40, 0.4f, -0.2f, "The Simulacra (1964)");
      CoolPrint1(*FontArial, 20,    t, 13.0f, 14.0f, 19.0f, 20.0f, 220, 280, 40, 0.4f, -0.2f, "Clans of the Alphane Moon (1964)");
      CoolPrint1(*FontArial, 20,    t, 14.0f, 15.0f, 20.0f, 21.0f, 200, 300, 40, 0.4f, -0.2f, "The Penultimate Truth (1964)");

      CoolPrint1(*FontArial, 20,    t, 15.0f, 16.0f, 21.0f, 22.0f, 290, 320, 40, 0.4f, -0.2f, "The tree stigmata of Palmer Eldritch (1965)");
      CoolPrint1(*FontArial, 20,    t, 16.0f, 17.0f, 22.0f, 23.0f, 145, 340, 40, 0.4f, -0.2f, "Dr.Bloodmoney (1965)");
      CoolPrint1(*FontArial, 20,    t, 17.0f, 18.0f, 23.0f, 24.0f, 180, 360, 40, 0.4f, -0.2f, "The crack in space (1966)");
      CoolPrint1(*FontArial, 20,    t, 18.0f, 19.0f, 24.0f, 25.0f, 190, 380, 40, 0.4f, -0.2f, "No wait for last year (1966)");

      CoolPrint1(*FontArial, 20,    t, 19.0f, 20.0f, 25.0f, 26.0f, 200, 400, 40, 0.4f, -0.2f, "The unteleported man (1966)");
      CoolPrint1(*FontArial, 20,    t, 20.0f, 21.0f, 26.0f, 27.0f, 195, 420, 40, 0.4f, -0.2f, "Counter clock world (1967)");
      CoolPrint1(*FontArial, 20,    t, 21.0f, 22.0f, 27.0f, 28.0f, 135, 20, 40, 0.4f, -0.2f, "The zap gun (1967)");
      CoolPrint1(*FontArial, 20,    t, 22.0f, 23.0f, 28.0f, 29.0f, 210, 40, 40, 0.4f, -0.2f, "The Ganymede takeover (1967)");

      CoolPrint1(*FontArial, 20,    t, 23.0f, 24.0f, 29.0f, 30.0f, 310, 60, 40, 0.4f, -0.2f, "Do androids dream of electric sheeps? (1968)");
      CoolPrint1(*FontArial, 20,    t, 24.0f, 25.0f, 30.0f, 31.0f, 85, 80, 40, 0.4f, -0.2f, "Ubik (1969)");
      CoolPrint1(*FontArial, 20,    t, 25.0f, 26.0f, 31.0f, 32.0f, 190, 100, 40, 0.4f, -0.2f, "Galactic Pot-Healer (1969)");
      CoolPrint1(*FontArial, 20,    t, 26.0f, 27.0f, 32.0f, 33.0f, 145, 120, 40, 0.4f, -0.2f, "Maze of death (1970)");

      CoolPrint1(*FontArial, 20,    t, 27.0f, 28.0f, 33.0f, 34.0f, 220, 140, 40, 0.4f, -0.2f, "Our friends from Frolix 8 (1970)");
      CoolPrint1(*FontArial, 20,    t, 28.0f, 29.0f, 34.0f, 35.0f, 165, 160, 40, 0.4f, -0.2f, "We can build you (1972)");
      CoolPrint1(*FontArial, 20,    t, 29.0f, 30.0f, 35.0f, 36.0f, 275, 180, 40, 0.4f, -0.2f, "Flow my tears the policeman said (1974)");
      CoolPrint1(*FontArial, 20,    t, 30.0f, 31.0f, 36.0f, 37.0f, 240, 200, 40, 0.4f, -0.2f, "Confessions of a crap artist (1975)");

      CoolPrint1(*FontArial, 20,    t, 31.0f, 32.0f, 37.0f, 38.0f, 125, 220, 40, 0.4f, -0.2f, "Deus irae (1976)");
      CoolPrint1(*FontArial, 20,    t, 32.0f, 33.0f, 38.0f, 39.0f, 165, 240, 40, 0.4f, -0.2f, "A scanner darkly (1977)");
      CoolPrint1(*FontArial, 20,    t, 33.0f, 34.0f, 39.0f, 40.0f, 85, 260, 40, 0.4f, -0.2f, "Valis (1981)");
      CoolPrint1(*FontArial, 20,    t, 34.0f, 35.0f, 40.0f, 41.0f, 185, 280, 40, 0.4f, -0.2f, "The divine Invasion (1981)");

      CoolPrint1(*FontArial, 20,    t, 35.0f, 36.0f, 41.0f, 42.0f, 305, 300, 40, 0.4f, -0.2f, "The trasmigrating of Timothy Archer (1982)");
      CoolPrint1(*FontArial, 20,    t, 36.0f, 37.0f, 42.0f, 43.0f, 285, 320, 40, 0.4f, -0.2f, "The man whose ttet where all alike (1984)");
      CoolPrint1(*FontArial, 20,    t, 37.0f, 38.0f, 43.0f, 44.0f, 190, 340, 40, 0.4f, -0.2f, "Radio free Albemuth (1985)");
      CoolPrint1(*FontArial, 20,    t, 38.0f, 39.0f, 44.0f, 45.0f, 265, 360, 40, 0.4f, -0.2f, "Puttering about in a small land (1985)");

      CoolPrint1(*FontArial, 20,    t, 39.0f, 40.0f, 45.0f, 46.0f, 220, 380, 40, 0.4f, -0.2f, "In Milton Lumky Territoru (1985)");
      CoolPrint1(*FontArial, 20,    t, 40.0f, 41.0f, 46.0f, 47.0f, 230, 400, 40, 0.4f, -0.2f, "Humpy dumpty in Hoackland (1986)");
      CoolPrint1(*FontArial, 20,    t, 41.0f, 42.0f, 47.0f, 48.0f, 185, 420, 40, 0.4f, -0.2f, "Mary and the Giant (1987)");
      CoolPrint1(*FontArial, 20,    t, 42.0f, 43.0f, 48.0f, 49.0f, 170, 440, 40, 0.4f, -0.2f, "The broken bubble (1988)");

      CoolPrint1(*FontArial, 20,    t, 43.0f, 44.0f, 49.0f, 50.0f, 220, 460, 40, 0.4f, -0.2f, "Gather yourself together (1994)");

      CoolPrint1(*FontArial, 20,    t, 50.0f, 51.0f, 5000.0f, 5000.0f, 300, 230, 40, 0.4f, -0.2f, "are you dreaming of electric sheeps ?");
  }

  skSwappuffers();

  skTimerFrame();

}

void skInitDemoStuff()
{

// pan stuff

#ifdef WIN32
  FontArial = new GLFont(hDC, "Arial");
#else
  glfInit();
  glfSetMemoryLoad();
  static GLF_MEMFILE glf_memfile = GLF_INIT_MEMFILE(arial_black_glf);
  //static GLF_MEMFILE glf_memfile = GLF_INIT_MEMFILE(arial1_glf);
  FontArial   = new GLFont(&glf_memfile);
#endif


	glClearColor (0.0f, 0.0f, 0.0f, 0.5f);						// Black Background
	glClearDepth (1.0f);										// Depth Buffer Setup
	glDepthFunc (GL_LEQUAL);									// The Type Of Depth Testing (Less Or Equal)
	glEnable(GL_DEPTH_TEST);									// Enable Depth Testing
	glShadeModel (GL_SMOOTH);									// Select Smooth Shading
//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Set Perspective Calculations To Most Accurate
	glDisable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	//glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	//glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

	glClear(GL_COLOR_BUFFER_BIT);

  //inizializzo i valori di defautl del particle system
  //e le relative textures

	skSwappuffers();


  //inizializzo lo sfondo
  m_glqMyQuadratic = gluNewQuadric();
  gluQuadricNormals(m_glqMyQuadratic, GLU_SMOOTH);
  gluQuadricTexture(m_glqMyQuadratic, GL_TRUE);

  env = perlin(3, 4, 0.6f, 0.4f, 1, true);
  frame = perlin(8, 1, 0.3f, 0.7f, 2, true); // ma cristodundio! I parametri no a caso! - modificato in 8
  frame2 = perlin(8, 1, 0.3f, 0.7f, 2, true); // ma cristodundio! I parametri no a caso! - modificato in 8
  //int a = frame2->getSize();
  gauss = perlin(3, 1, 0.3f, 0.7f, 2, true);

  toro = perlin(4, 5, 0.4f, 0.5f, 1.5, true);
	cubo = perlin(5, 16, 0.2f, 0.3f, 2.2f, true);
	texture1 = perlin(8,3000,0.3f,0.5, 1.8f, false);
	texture2 = perlin(8,300,0.1f,0.97f, 3, false);
	mytek = perlin(4,16,0.4f, 0.5f, 3, true);
	tex = perlin(3, 1, 0.4f, 0.5f, 2, true);
	tex2 = perlin(8, 132, 0.3f, 0.7f, 2, true);
	scritte = perlin(7, 100, 0.6f, 0.5f, 1.3f, true);// usare per le scritte

  //LOD, radius
  initSphereObject(40,2);

  logo = CreatePKLogoTexture();
}

void skUnloadDemoStuff()
{
  gluDeleteQuadric(m_glqMyQuadratic);

  glDeleteTextures(1,&logo);
  unloadSphereEffect();

  //delete [] env;
}

#ifdef __LINUX__
/*
 * Switches:
 *
 * -w       force windowed mode (default is fullscreen)
 * -d bpp   force depth (bits per pixels, normally 32)
 * -PK      hidden part
 */
int flag_w = 0;
int flag_hidden = 0;
int flag_d_value = 0;

int main(int argc, char *argv[]) {
    int c;

    while ((c = getopt (argc, argv, "?PKwd:")) != -1)
        switch (c)
        {
	case 'P':
	    flag_hidden = 1;
	    break;
	case 'K':
	    if (flag_hidden) flag_hidden = 2;
	    break;
	case 'w':
	    flag_w = 1;
	    break;
	case 'd':
	    {
		char *tail;
		if (optarg != nullptr) flag_d_value = strtol(optarg,&tail,0);
		if (optarg == nullptr || tail == optarg || *tail != '\0') {
		    fprintf(stderr, "Invalid display depth (bits per pixel)\n");
		    exit(1);
		}
	    }
	    break;
	case '?':
	default:
	    fprintf (stderr,
		     "\nPK is dead\n"
		     "  -w        force windowed mode\n"
		     "  -d bpp    set display depth (defaults to let SDL decide)\n\n");
	    exit(0);
	    break;
        }

    if (flag_hidden != 2) flag_hidden = 0;
    if (flag_hidden) hiddenpart = true; else hiddenpart = false;
    

    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
	fprintf(stderr, "Couldn't init SDL: %s\n", SDL_GetError());
	exit(1);
    }

    if ( SDL_SetVideoMode(WIDTH, HEIGHT, flag_d_value, SDL_OPENGL | (!flag_w * SDL_FULLSCREEN)) == nullptr) {
	fprintf(stderr, "Unable to create OpenGL screen: %s\n", SDL_GetError());
	SDL_Quit();
	exit(2);
    }

    SDL_WM_SetCaption("PK is dead", nullptr);
    SDL_ShowCursor(0);

    done = 0;
#endif /* __LINUX__ */

#ifdef WIN32
int WINAPI WinMain(HINSTANCE hinstance,HINSTANCE hprevinstance,LPSTR lpcmdline,int ncmdshow) {

  WNDCLASSEX		winclass;
  HDC				l_hDC;

  if (strcmp(lpcmdline,"/PK") == 0)
    hiddenpart = true;
  else
    hiddenpart = false;

	hInstance = hinstance;
	l_hDC=GetDC(GetDesktopWindow());
  
	winclass.cbSize			= sizeof(WNDCLASSEX);
	winclass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC; //CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc	= (WNDPROC)(skWinProc);
	winclass.cbClsExtra		= 0;
	winclass.cbWndExtra		= 0;
	winclass.hInstance		= hinstance;
	winclass.hIcon			= LoadIcon(nullptr, IDI_APPLICATION);
	winclass.hIconSm		= LoadIcon(nullptr, IDI_APPLICATION);
	winclass.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	winclass.hbrBackground	= (HBRUSH)(COLOR_APPWORKSPACE);
	winclass.lpszMenuName	= nullptr;
	winclass.lpszClassName	= "WINDOW_CLASS";

	if (!RegisterClassEx(&winclass)) 
    return 0;

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd 
		1,                                // version number 
		PFD_DRAW_TO_WINDOW |              // support window 
		PFD_SUPPORT_OPENGL |              // support OpenGL 
		PFD_DOUBLEBUFFER,
//PFD_GENERIC_ACCELERATED,            // double buffered 
		PFD_TYPE_RGBA,                    // RGBA type 
		BITSPERPIXEL,                     // color depth 
		0, 0, 0, 0, 0, 0,                 // color bits ignored 
		1,                                // no alpha buffer 
		0,                                // shift bit ignored 
		0,                                // no accumulation buffer 
		0, 0, 0, 0,                       // accum bits ignored 
		16,                               // 32-bit z-buffer     
		1,                                // stencil buffer 
		0,                                // no auxiliary buffer 
		PFD_MAIN_PLANE,                   // main layer 
		0,                                // reserved 
		0, 0, 0                           // layer masks ignored 
	}; 
	
	unsigned int  iPixelFormat; 

	RECT windowRect = {0, 0, WIDTH, HEIGHT};	// Define Our Window Coordinates
  DWORD windowStyle, windowExtendedStyle;

#ifdef FULLSCREEN
  int oldwidth = GetDeviceCaps(l_hDC, HORZRES);
  int oldheight = GetDeviceCaps(l_hDC, VERTRES);
  int oldbpp = GetDeviceCaps(l_hDC, BITSPIXEL);
  int oldfreq = GetDeviceCaps(l_hDC, VREFRESH);

	DEVMODE dmScreenSettings;											// Device Mode
	ZeroMemory (&dmScreenSettings, sizeof (DEVMODE));					// Make Sure Memory Is Cleared
	dmScreenSettings.dmSize				= sizeof (DEVMODE);				// Size Of The Devmode Structure
	dmScreenSettings.dmPelsWidth		= WIDTH;						// Select Screen Width
	dmScreenSettings.dmPelsHeight		= HEIGHT;						// Select Screen Height
	dmScreenSettings.dmBitsPerPel		= BITSPERPIXEL;					// Select Bits Per Pixel
	dmScreenSettings.dmFields			= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
  if (ChangeDisplaySettings (&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
    MessageBox(0, "FullScreen mode not available", WINDOWTITLE, MB_OK);
  	windowStyle = WS_OVERLAPPEDWINDOW;
  	windowExtendedStyle = WS_EX_APPWINDOW;
    AdjustWindowRectEx (&windowRect, windowStyle, 0, windowExtendedStyle);
  } else {
  	ShowCursor(false);
  	windowStyle = WS_POPUP;
  	windowExtendedStyle = WS_EX_APPWINDOW | WS_EX_TOPMOST;
  }
#else
	windowStyle = WS_OVERLAPPEDWINDOW;
	windowExtendedStyle = WS_EX_APPWINDOW;
  AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExtendedStyle);
#endif
	if (hWND = CreateWindowEx (windowExtendedStyle,					// Extended Style
								   WINDOW_CLASS_NAME,	// Class Name
								   WINDOWTITLE,					// Window Title
								   windowStyle,							// Window Style
								   0, 0,								// Window X,Y Position
								   windowRect.right - windowRect.left,	// Window Width
								   windowRect.bottom - windowRect.top,	// Window Height
								   HWND_DESKTOP,						// Desktop Is Window's Parent
								   nullptr,									// No Menu
								   hInstance, // Pass The Window Instance
        nullptr)) {
  	hDC = GetDC (hWND);

  	iPixelFormat = ChoosePixelFormat(hDC, &pfd);


  	// make that match the device context's current pixel format 
  	SetPixelFormat(hDC, iPixelFormat, &pfd); 
  	
  	// if we can create a rendering context ...  
  	if (hRC = wglCreateContext(hDC)) { 
  		// try to make it the thread's current rendering context 
  		wglMakeCurrent(hDC, hRC); 
  	} 

  	ShowWindow(hWND, SW_NORMAL);								// Make The Window Visible
#endif /* WIN32 */

    FSOUND_File_SetCallbacks(memopen, memclose, memread, memseek, memtell);
    if (!FSOUND_Init(44100, 0)) 
    {
      //why quit if no audio available ??  (rIO)
      //PostQuitMessage(0); 
      isMusicEnabled = false;
    }
    if (isMusicEnabled)
    {
#ifdef WIN32
        fmodule = FMUSIC_LoadSong(MAKEINTRESOURCE(IDR_RC_MUSIC), nullptr);
#else
        fmodule = FMUSIC_LoadSong("cippa", nullptr);
#endif
    }

    if (!fmodule) 
    {
      //why quit if no audio available ??  (rIO)
      //PostQuitMessage(0); //g_isMusicEnabled = false;
      isMusicEnabled = false;
    }


    skInitDemoStuff();
    if (isMusicEnabled)
      FMUSIC_PlaySong(fmodule);  
    skInitTimer();

#ifdef WIN32
    while (true) {
  		MSG msg;
  		if (PeekMessage(&msg, nullptr,0,0,PM_REMOVE)) {
  			if (msg.message == WM_QUIT) 
          break;
  			TranslateMessage(&msg);
  			DispatchMessage(&msg);
  		} else
  			skDraw();
  	}
#else
    while ( ! done ) {
	skDraw();
        {
            SDL_Event event;
            while ( SDL_PollEvent(&event) ) {
                if ( event.type == SDL_QUIT ) {
                    done = 1;
                }
                if ( event.type == SDL_KEYDOWN ) {
                    if ( event.key.keysym.sym == SDLK_ESCAPE ||
			 event.key.keysym.sym == SDLK_SPACE  ||
			 event.key.keysym.sym == 65          ||
			 event.key.keysym.sym == 9) { // strange things happen...
                        done = 1;
                    }
#ifdef SCREENSHOOTER
		    if ( event.key.keysym.sym == SDLK_s ||
			 event.key.keysym.sym == 39) {
			system("screenshooter.sh 0x1c0000e"); // this is utterly lame :(
		    }
#endif /* SCREENSHOOTER */
                }
            }
        }
    }
#endif

    if (isMusicEnabled)
    {
  	  FMUSIC_FreeSong(fmodule);
  	  FSOUND_Close();
    }

#ifdef WIN32
  } 
  else
    MessageBox(GetDesktopWindow(), "Can't create window", "SKerror", MB_OK);
#endif /* WIN32 */

#ifdef WIN32
#ifdef FULLSCREEN
  static DEVMODE mode;
  mode.dmSize=sizeof(mode);
  mode.dmBitsPerPel=oldbpp;
  mode.dmPelsWidth=oldwidth;
  mode.dmPelsHeight=oldheight;
  mode.dmDisplayFrequency = oldfreq;
  mode.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;
  ChangeDisplaySettings(&mode, 0);
	ShowCursor(true);
#endif
#endif /* WIN32 */

//  if (!isMusicEnabled) 
//    MessageBox(GetDesktopWindow(), "Sound Device not found", "SKerror", MB_OK);


  skUnloadDemoStuff();

#ifdef WIN32
	ExitProcess(0);
#else
	SDL_Quit();
#endif
  return 0;
}


