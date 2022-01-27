
/************************************
  Code By: Pan/SpinningKids
  Comments: Creation
 ************************************/

#include "GenTex.h"
#include "noise.h"
#include <cmath>

GLTexture* perlin(int logsize, float freq, float amp, float base, float k, bool wrap) {
    GLTexture* prln = new GLTexture(logsize);
    int size = prln->getSize();
    float* tex = prln->getImage();
    int fillsize = size;
    if (wrap) {
        fillsize >>= 1;
    }
    float m = freq / fillsize;
    for (int i = 0; i < fillsize; i++) {
        for (int j = 0; j < fillsize; j++) {
            float col = vnoise(m * i, m * j) * amp + base;
            col = (col < 0) ? 0 : (col > 1) ? 1 : col;
            tex[(((i << logsize) + j) << 2) + 0] = powf(col, 1.f / k);
            tex[(((i << logsize) + j) << 2) + 1] = col;
            tex[(((i << logsize) + j) << 2) + 2] = powf(col, k);
            tex[(((i << logsize) + j) << 2) + 3] = 1;
        }
    }
    if (wrap) {
        for (int i = 0; i < fillsize; i++) {
            for (int j = 0; j < fillsize; j++) {
                for (int k = 0; k < 4; k++) {
                    tex[((((size - 1 - i) << logsize) + j) << 2) + k] = tex[(((i << logsize) + size - 1 - j) << 2) + k] = tex[((((size - 1 - i) << logsize) + size - 1 - j) << 2) + k] = tex[(((i << logsize) + j) << 2) + k];
                }
            }
        }
    }
    prln->update();
    return prln;
}
