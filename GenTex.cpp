
#include "GenTex.h"
#include "noise.h"
#include <math.h>

float gaussian(float x, float y, float sigma)
{
#define SQRT2PI 2.506628274631000502415765284811045253006
	//return exp(-0.5 * (x*x + y*y) / (sigma*sigma)) / (SQRT2PI * sigma);
  float c = sigma / 2;
  double diag = sqrt((sigma * sigma)*2) / 2;
  return 1 - sqrt((x-c)*(x-c) + (y-c)*(y-c)) / diag;
}

GLTexture *spot(float psize, float strenght)
{
  GLTexture *spt = new GLTexture((int)psize);
  int size = spt->getSize();
  GLfloat *tex = spt->getImage();

  int fillsize = size;

  for(int x = 0; x < fillsize; x++) 
  {
    for(int y = 0; y < fillsize; y++) 
    {
      float col = gaussian(x,y,size) * strenght;
      col = (col < 0) ? 0: (col > 1) ? 1: col;
      tex[(y * size) + x +0] = col;
      tex[(y * size) + x +1] = col;
      tex[(y * size) + x +2] = col;
      tex[(y * size) + x +3] = col;
    }
  }
 
  spt->update();
  return spt;
}

GLTexture *perlin(int logsize, float freq, float amp, float base, float k, bool wrap) {
  GLTexture *prln = new GLTexture(logsize);
  int size = prln->getSize();
  GLfloat *tex = prln->getImage();
  int fillsize = size;
  if (wrap) {
    fillsize >>= 1;
  }
  float m = freq/fillsize;
  float ncoord[3];
  for(int i = 0; i < fillsize; i++) {
    ncoord[0] = m*i;
    for(int j = 0; j < fillsize; j++) {
      ncoord[1] = m*j;
      float col = vnoise(2, ncoord)*amp+base;
      col = (col < 0) ? 0: (col > 1) ? 1: col;
      tex[(((i<<logsize)+j)<<2)+0] = pow(col, 1.f/k);
      tex[(((i<<logsize)+j)<<2)+1] = col;
      tex[(((i<<logsize)+j)<<2)+2] = pow(col, k);
      tex[(((i<<logsize)+j)<<2)+3] = 1;
    }
  }
  if (wrap)
    for(int i = 0; i < fillsize; i++)
      for(int j = 0; j < fillsize; j++)
        for(int k = 0; k < 4; k++)
          tex[((((size-1-i)<<logsize)+j)<<2)+k] = tex[(((i<<logsize)+size-1-j)<<2)+k] = tex[((((size-1-i)<<logsize)+size-1-j)<<2)+k] = tex[(((i<<logsize)+j)<<2)+k];
  prln->update();
  return prln;
}
