
#define WIN32_LEAN_AND_MEAN

constexpr char WINDOW_CLASS_NAME[] = "SKCLASS";

/*##########################################################*/
/* Standard includes										*/
/*##########################################################*/
//TEMP cleanme
#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#include "AsmMath4.h"
#else
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <unistd.h>
#endif

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

/*##########################################################*/
/* Sound Player includes									*/
/*##########################################################*/

typedef unsigned char byte;
typedef unsigned short word;


constexpr int NUTSH = 32;
constexpr int NUTSV = 16;

constexpr int QUADV = 20;
constexpr int QUADH = 20;

GLFont *FontArial;
CParticleSystem parts1;
CParticleSystem parts2;
GLUquadricObj*   m_glqMyQuadratic;
bool donerendertotexture = false;

//vars for nuts speedup
Vector3 vnuts[NUTSH * NUTSV * 6];  //V*H*6 vertices
Vector3 nnuts[NUTSH * NUTSV * 6];  //normals
int vind = 0;

//vars for texturedquads speedup
Vector3 vquad[QUADV * QUADH * 4];  //V*H*6 vertices
Vector3 nquad[QUADV * QUADH * 4];  //normals
uv_coord tquad[QUADV * QUADH * 4]; //texturecoords
int qind = 0;

GLTexture* env; //genric envy
GLTexture* frame; //used by render to texture
GLTexture* frame2; //used by render to texture
GLTexture* toro;
GLTexture* cubo;
GLTexture *texture1;
GLTexture *texture2;
GLTexture* tex;
GLTexture* tex2;
GLTexture* scritte;

GLuint logo;

//solo per provare...
constexpr float timebase[19] = {0.f, 10.705f, 11.632f, 14.54f, 17.084f, 23.273f, 28.734f, 34.7f, 35.821f, 36.348f, 47.987f, 58.458f, 72.715f, 84.340f, 95.961f, 107.617f, 117.785f, 119.240f}; // 121 fu 119.240};

constexpr int twirlstable[]{ 7, 1, 6, 2, 8, 1, 7, 1 };

/*##########################################################*/
/*Sound Player Implementation								*/
/*##########################################################*/
float g_delta;

//This just sets the states/matrices needed to render in a texture
void PrepareRenderToTexture(float FOV,int size) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective((double)FOV, 1.0, 0.1, 600.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glViewport(0, 0, size, size);
    glLoadIdentity();
}


void DoRenderToTexture(GLTexture* tex) {
    glFlush();
    tex->use();
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, tex->getSize(), tex->getSize(), 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, WIDTH, HEIGHT);
    donerendertotexture = true;
}

//function that evaluates the pos vor a vertex on a cassini oval
Vector3 EvalNut(float t, float a, float b) {
   float a2 = a * a;
   float b2 = b * b;
   float a2s = a2 * sinf(2 * t);
   float c1 = b2 * b2 - a2s * a2s;
   float c2 = sqrtf(a2 * cosf(2 * t) + (c1 <= 0 ? 0 : sqrtf(c1)));

   return{ cosf(t) * c2 - ((a > b) ? a : 0), sinf(t) * c2, 0.0 };
}

//Just draws a cube in the origin
//Parameters:
//size      : the cube size
void dCube(const Vector3 &size) {
    Vector3 min = -size / 2.f;
    Vector3 max = size / 2.f;

    glBegin(GL_QUADS);
    glColor4f(1.f, 1.f, 1.f, 0.6f);
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

//- Evaluate, Load Arrays and Draws the Cassini oval
//Params:
//value : distance from oval centers
//recalc : recalc the vertex arrays (to be done one time per frame)
//flatten : if enabled flatten the ovals UNDER the fval
//fval : if flatten is enabled the ovals are flatten under this value
//
void dNuts(float value, bool recalc, bool flattenonfloor, float flattenval, rgb_a col) {
    Vector3 zperp{ 0, 0, 1 };

    //the calc mess is just to speed up things (sorry)
    //just call this func with calc = true once per frame

    if (recalc) {

        //index for arrays
        vind = 0;

        float a = fabsf(sinf(value));
        float b = 1.f;

        for (int j = 0; j < NUTSV; j++) {
            float theta1 = TWOPI * j / NUTSV;
            float theta2 = TWOPI * ((j + 1) % NUTSV) / NUTSV;

            for (int i = 0; i < NUTSH; i++) {
                float t1 = PI * i / NUTSH;
                float t2 = PI * (i + 1) / NUTSH;

                // faccia dell'ovale
                Vector3 p0 = EvalNut(t1, a, b);
                Vector3 n0 = (p0 - EvalNut(t1 + 0.1f / NUTSH, a, b)) ^ zperp; //normale

                Vector3 p1 = EvalNut(t2, a, b);
                Vector3 n1 = (EvalNut(t2 - 0.1f / NUTSH, a, b) - p1) ^ zperp; //normale

                //the following part rotates around the oval axis
                float ct1 = cosf(theta1);
                float st1 = sinf(theta1);
                float ct2 = cosf(theta2);
                float st2 = sinf(theta2);

                Vector3 p[4]{
                    { p0.x, p0.y * ct1 + p0.z * st1, -p0.y * st1 + p0.z * ct1 },
                    { p1.x, p1.y * ct1 + p1.z * st1, -p1.y * st1 + p1.z * ct1 },
                    { p1.x, p1.y * ct2 + p1.z * st2, -p1.y * st2 + p1.z * ct2 },
                    { p0.x, p0.y * ct2 + p0.z * st2, -p0.y * st2 + p0.z * ct2 } };

                Vector3 n[4]{
                    Vector3{ n0.x, n0.y * ct1 + n0.z * st1, -n0.y * st1 + n0.z * ct1 }.Normalized(),
                    Vector3{ n1.x, n1.y * ct1 + n1.z * st1, -n1.y * st1 + n1.z * ct1 }.Normalized(),
                    Vector3{ n1.x, n1.y * ct2 + n1.z * st2, -n1.y * st2 + n1.z * ct2 }.Normalized(),
                    Vector3{ n0.x, n0.y * ct2 + n0.z * st2, -n0.y * st2 + n0.z * ct2 }.Normalized() };


                if (flattenonfloor) {
                    for (int np = 0; np < 4; np++) {
                        if (p[np].y < flattenval)
                            p[np].y = flattenval;
                        n[np].y = -1;
                    }
                }

                if (!(p[0] == p[1]) && !(p[1] == p[2]) && !(p[2] == p[0])) {
                    vnuts[vind] = p[0];
                    nnuts[vind] = n[0];
                    vind++;

                    vnuts[vind] = p[1];
                    nnuts[vind] = n[1];
                    vind++;

                    vnuts[vind] = p[2];
                    nnuts[vind] = n[2];
                    vind++;
                }
                if (!(p[0] == p[2]) && !(p[2] == p[3]) && !(p[3] == p[0])) {
                    vnuts[vind] = p[0];
                    nnuts[vind] = n[0];
                    vind++;

                    vnuts[vind] = p[2];
                    nnuts[vind] = n[2];
                    vind++;

                    vnuts[vind] = p[3];
                    nnuts[vind] = n[3];
                    vind++;
                }
            }
        }
    }

    glColor4f(col.r, col.g, col.b, col.a);

    glVertexPointer(3, GL_FLOAT, 0, &vnuts);
    glNormalPointer(GL_FLOAT, 0, &nnuts);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glDrawArrays(GL_TRIANGLES, 0, vind);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

}

void dHelix(float outr, float inr, int twists, int angle_steps, rgb_a startcol, rgb_a endcol) {
    static GLuint springlist = 0;

    glBegin(GL_QUADS);
    for (int phi = 0; phi <= 360; phi += angle_steps)
    {
        float v0 = phi * PIOVER180;
        float v1 = (phi + angle_steps) * PIOVER180;
        float cv0 = (cosf(v0) + inr) * outr;
        float sv0 = sinf(v0) - TWOPI;
        float cv1 = (cosf(v1) + inr)* outr;
        float sv1 = sinf(v1) - TWOPI;

        for (int theta = 0; theta <= 360 * twists; theta += angle_steps)
        {
            float u0 = theta * PIOVER180;
            float u1 = (theta + angle_steps) * PIOVER180;
            float cu0 = cosf(u0);
            float su0 = sinf(u0);
            float cu1 = cosf(u1);
            float su1 = sinf(u1);

            Vector3 vertices[4]{
                {cu0 * cv0, su0 * cv0, (u0 + sv0) * outr},
                {cu1 * cv0, su1 * cv0, (u1 + sv0) * outr},
                {cu1 * cv1, su1 * cv1, (u1 + sv1) * outr},
                {cu0 * cv1, su0 * cv1, (u0 + sv1) * outr}
            };

            Vector3 normal = CalcNormal(vertices[2], vertices[0], vertices[1]);

            glNormal3fv((float*)&normal);

            rgb_a col = GetFade(startcol, endcol, (theta + 1.f) / (360.f * (float)twists));

            glColor4fv((float*)&col);
            glVertex3fv((float*)&vertices[0]);
            glVertex3fv((float*)&vertices[1]);
            glVertex3fv((float*)&vertices[2]);
            glVertex3fv((float*)&vertices[3]);
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
void dCylinder(float r, int segsh, int segsv, float length, float wobble, float wdelta, float wfreq, float mytime, bool fill, rgb_a color, rgb_a furcolor, float furlenght) {
    float horstep = TWOPI / segsh;
    float vertstep = length / segsv;


    if (fill) {
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glDisable(GL_LIGHTING);
        //glColor4f(0.9f, 0.9f, 0.9f, 0.1f);
        glColor4fv((float*)&color);

        glBegin(GL_QUADS);

        for (int x = 0; x < segsh; x++) {
            float sinfa = sinf(x * horstep);
            float cosfa = cosf(x * horstep);
            float sinfb = sinf((x + 1) * horstep);
            float cosfb = cosf((x + 1) * horstep);
            for (int y = 0; y < segsv; y++) {
                float yfa = y * vertstep - (length / 2);
                float anglea = yfa * wfreq * TWOPI / length + wdelta;
                float rwoba = r + sinf(anglea) + cosf(anglea) * wobble;
                float yfb = yfa + vertstep;
                float angleb = yfb * wfreq * TWOPI / length + wdelta;
                float rwobb = r + sinf(angleb) + cosf(angleb) * wobble;
                //  |_
                glNormal3f(sinfa, 0.f, -cosfa);
                glVertex3f(cosfa * rwoba, yfa, sinfa * rwoba);

                //   _
                //  |
                glVertex3f(cosfa * rwobb, yfb, sinfa * rwobb);

                //   _
                //    |
                glNormal3f(sinfb, 0, -cosfb);
                glVertex3f(cosfb * rwobb, yfb, sinfb * rwobb);

                //  _|
                glVertex3f(cosfb * rwoba, yfa, sinfb * rwoba);
            }
        }

        glEnd();

        GLTexture::disable();
        glEnable(GL_BLEND);
        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
    }

    int ch = segsh * 4;
    int cv = segsv / 2;
    for (int ix = 0; ix < ch; ix++) {
        for (int iy = 0; iy < cv; iy++) {
            float x = (TWOPI * (vnoise(mytime, vlattice(ix, iy, 1)) + ix)) / ch;
            float cosfx = cosf(x);
            float sinfx = sinf(x);
            //  |_
            float v = (length * (vnoise(mytime, vlattice(ix, iy, 2)) + iy)) / cv - (length / 2.f);
            float angle = v * wfreq * TWOPI / length;
            float wob = sinf(angle + wdelta) + cosf(angle + wdelta) * wobble;
            glColor4f(furcolor.r, furcolor.g, furcolor.b, 0.5f);
            glVertex3f(cosfx * (r + wob), v, sinfx * (r + wob));

            //  |_
            glColor4f(furcolor.r, furcolor.g, furcolor.b, 0.f);
            glVertex3f(cosfx * ((r * furlenght) + wob), v + wfreq * TWOPI * (cosf(angle) - wobble * sinf(angle)) / length, sinfx * ((r * furlenght) + wob));
        }
    }

    glEnd();

}

uv_coord TwirlTexCoords(int x, int y, float value) {
    float xf = (x - QUADV / 2);
    float yf = (y - QUADH / 2);
    float r = sqrtf(xf * xf + yf * yf);
    float a = atan2f(xf, yf);
    float r2 = r + 5 * (1 + sinf(value * 3));
    float a2 = value + a + 0.5f * sinf(r / 8 + value * 1.5f) + 0.55f * cosf(r / 4 + value * 1.9f);
    float tx = 0.75f * r2 * sinf(a2) / QUADV;
    float ty = 0.75f * r2 * cosf(a2) / QUADH;
    return { tx - 0.5f, ty - 0.5f };
}

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
void dTorus(float time, float r0, float r1, int n, bool deform) {
    uv_coord t;


    if (r1 < 0)
        r1 = -r1;
    if (r0 < 0)
        r0 = -r0;
    if (n < 0)
        n = -n;

    for (int j = 0; j < n; j++) {
        float t1 = j * TWOPI / n;
        float t2 = (j + 1) * TWOPI / n;
        float cosft1 = cosf(t1);
        float sinft1 = sinf(t1);
        float cosft2 = cosf(t2);
        float sinft2 = sinf(t2);

        glBegin(GL_QUAD_STRIP);

        for (int i = 0; i <= n; i++) {

            float tmpr1 = r1 + sinf((float)i) / 5 + sinft2 * (1 + sinf(j / 8.f + time * 3));

            float phi = i * TWOPI / n;
            float tmpr1cosfphi = tmpr1 * cosf(phi);
            float tmpr1sinfphi = tmpr1 * sinf(phi);

            Vector3 e0 = Vector3{ cosft1 * tmpr1cosfphi, tmpr1sinfphi, sinft1 * tmpr1cosfphi }.Normalized();
            Vector3 p0{ cosft1 * (r0 + tmpr1cosfphi), tmpr1sinfphi, sinft1 * (r0 + tmpr1cosfphi) };

            t.u = i / (float)n;
            t.v = j / (float)n;

            Vector3 e1 = Vector3{ cosft2 * tmpr1cosfphi, tmpr1sinfphi, sinft2 * tmpr1cosfphi }.Normalized();
            Vector3 p1{ cosft2 * (r0 + tmpr1cosfphi), tmpr1sinfphi, sinft2 * (r0 + tmpr1cosfphi) };

            t.u = i / (float)n;
            t.v = (j + 1) / (float)n; // BUG: This should be moved after glTexCoord2f

            glNormal3f(e0.x, e0.y, e0.z);
            glTexCoord2f(t.u, t.v);
            glVertex3f(p0.x, p0.y, p0.z + sinf(p0.x) * cosf(time) * 2.f);

            glNormal3f(e1.x, e1.y, e1.z);
            glTexCoord2f(t.u, t.v);
            glVertex3f(p1.x, p1.y, p1.z + sinf(p1.x) * cosf(time) * 2.f);

        }
        glEnd();
    }
}


// Return a texture coord displaved like a wave by a JLSAC
//x,y : original texture coords
// value : waving value (time)
// divisor : the resulting wave sin/cos displacement is divided by this value
uv_coord WaterTexCoords(int x, int y, float value, float divisor) {
    float tx = std::min(((float)x / QUADV) + sinf(x * value) / divisor, 1.f);
    float ty = std::min(((float)y / QUADH) + cosf(y * value) / divisor, 1.f);

    return { tx, ty };
}

void drawHelix(float angle, float inr, float outr, int twirls, int angstep, GLenum mode, rgb_a startcol, rgb_a endcol) {

    glEnable(GL_COLOR_MATERIAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    GLTexture::disable();
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

    if (mode == GL_LINE) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glLineWidth(1);
    }
}

//Draws a TexturedQuad with a waving effect on it
//Params:
//tex : a GLTexture object
// alpha : the alpha
// sx : the scale along x of the TEXTURE coords
// sy : the scale along Y of the TEXTURE coords
// calc : does the vertex array needs to be reloaded ? (once per frame)
// value : the value used to wave the texture
// divisor : the wave size divisor
void drawTexture(GLTexture* tex, float sx, float sy, bool calc, float value, float divisor, rgb_a color, bool water) {
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
    if (calc) {
        qind = 0;

        for (int y = 0; y < QUADV; y++) {
            for (int x = 0; x < QUADH; x++) {
                if (water)
                    tquad[qind] = WaterTexCoords(x, -y, value, divisor);
                else
                    tquad[qind] = TwirlTexCoords(x, -y, value);

                vquad[qind] = Vector3{ (float)x / QUADH * (float)WIDTH, (float)y / QUADV * (float)HEIGHT, 0 };
                qind++;

                if (water)
                    tquad[qind] = WaterTexCoords(x + 1, -y, value, divisor);
                else
                    tquad[qind] = TwirlTexCoords(x + 1, -y, value);

                vquad[qind] = Vector3{ (float)(x + 1) / QUADH * (float)WIDTH, (float)y / QUADV * (float)HEIGHT, 0 };
                qind++;

                if (water)
                    tquad[qind] = WaterTexCoords(x + 1, -(y + 1), value, divisor);
                else
                    tquad[qind] = TwirlTexCoords(x + 1, -(y + 1), value);

                vquad[qind] = Vector3{ (float)(x + 1) / QUADH * (float)WIDTH, (float)(y + 1) / QUADV * (float)HEIGHT, 0 };
                qind++;

                if (water)
                    tquad[qind] = WaterTexCoords(x, -(y + 1), value, divisor);
                else
                    tquad[qind] = TwirlTexCoords(x, -(y + 1), value);

                vquad[qind] = Vector3{ (float)x / QUADH * (float)WIDTH, (float)(y + 1) / QUADV * (float)HEIGHT, 0 };
                qind++;
            }
        }
    }

    glVertexPointer(3, GL_FLOAT, 0, &vquad);
    glTexCoordPointer(2, GL_FLOAT, 0, &tquad);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glDrawArrays(GL_QUADS, 0, qind);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glPopMatrix();

    glEnable(GL_COLOR_MATERIAL);

}

//draws the Cassini Oval scene
void drawNuts(float t) {
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

    glColor4f(1.f, 1.f, 1.f, 0.8f);
    //set a amtrix to render them slighty rotated and
    //translated going far away (IMHO looks good)
    glTranslatef(-2.2f, -0.3f, 0.f); // pan modifica - se sta sempre dentro lo schermo fa da cagare...
    glRotatef(-30, 0, 1, 0);
    glRotatef(sinf(t * 2) * 30.f, 0.f, 0.f, 1.f);
    glTranslatef(0, 0, -3);
    dNuts(t, true, false, 0, { 1.f, 1.f, 1.f, 0.4f });  //just loads the vertices once


    //glTranslatef(0,0,-5);
    glPushMatrix();
    glScalef(0.9f, 0.9f, 0.9f);
    dNuts(t, false, false, 0, { 1.f, 1.f, 1.f, 0.4f });
    glPopMatrix();

    glPushMatrix();
    glScalef(0.8f, 0.8f, 0.8f);
    dNuts(t, false, false, 0, { 1.f, 1.f, 1.f, 0.4f });
    glPopMatrix();

    glPopMatrix();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);

}

//draws the Tube scene
void drawTubo(int order, float t, rgb_a coltubo, rgb_a colpeli) {
    toro->use();

    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);


    constexpr float r = 2.f;
    constexpr int segsh = 16;
    constexpr int segsv = 80;
    constexpr float length = 80.f;
    constexpr float wobblepar = 0.5f;
    constexpr float freq = 10.f;

    float deltaw = t * 6.f;


    glEnable(GL_COLOR_MATERIAL);
    glPushMatrix();
    glPushMatrix();
    glTranslatef(0.f, 0.f, -10.f);
    glRotatef(90.f, 1.f, 0.f, 0.f);
    glRotatef(sinf(t) * 15.f, 0.f, 0.f, 1.f);
    glRotatef(sinf(t) * 15.f, 1.f, 0.f, 0.f);
    dCylinder(r, segsh, segsv, length, wobblepar, deltaw, freq, t, true, coltubo, colpeli, 1.5f);

    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.f, 0.f, -10.f);
    glRotatef(90.f, 1.f, 0.f, 0.f);
    glRotatef(sinf(t + 0.2f) * 15.f, 0.f, 0.f, 1.f);
    glRotatef(sinf(t + 0.2f) * 15.f, 1.f, 0.f, 0.f);
    dCylinder(r, segsh, segsv, length, wobblepar, deltaw, freq, t, true, coltubo, colpeli, 1.5f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.f, 0.f, -10.f);
    glRotatef(90.f, 1.f, 0.f, 0.f);
    glRotatef(sinf(t + 0.4f) * 15.f, 0.f, 0.f, 1.f);
    glRotatef(sinf(t + 0.4f) * 15.f, 1.f, 0.f, 0.f);
    dCylinder(r, segsh, segsv, length, wobblepar, deltaw, freq, t, true, coltubo, colpeli, 1.5f);
    glPopMatrix();

    glPopMatrix();

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glCullFace(GL_BACK);

}

//draws teh Torus scene
void drawToroide(int order, float t, float mytime) {
    toro->use();

    glDepthMask(GL_TRUE);
    glEnable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);

    glPushMatrix();

    if (order < 1)
        glColor4f(1.f, 1.f, 1.f, 0.2f);
    else //if (order == 2)
        glColor4f(1.f, 1.f, 1.f, 0.2f - 0.5f * ((skGetTime() - 11.2f) / 5.f));

    glDisable(GL_DEPTH_TEST);
    glTranslatef(0, 0, -10);
    glRotatef(sinf(t * 1.2f) * 140.f, 1.f, 0.f, 0.f);
    glRotatef(sinf(t * 1.7f) * 60.f, 0.f, 0.f, 1.f);
    dTorus(mytime, 2.f, 0.6f, 64, true);
    glTranslatef(0.f, 0.f, 0.01f);
    if (order < 2)
        glColor4f(1.f, 1.f, 1.f, 0.1f);
    else
        glColor4f(1.f, 1.f, 1.f, (0.5f * (0.2f - t)));
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    dTorus(mytime, 2.f, 0.6f, 64, true);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glPopMatrix();

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glCullFace(GL_BACK);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}


void drawWissEffect(const Vector3& pos, const Vector3& rot, const rgb_a& pcolor, const rgb_a& vcolor, bool base) {
    float fogColor[4] = { 0, 0, 0, 0 };

    GLTexture::disable();
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // fog stuff
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, 0.01f);
    glFogf(GL_FOG_START, -pos.z - 3.f);
    glFogf(GL_FOG_END, -pos.z);
    glHint(GL_FOG_HINT, GL_FASTEST);
    glEnable(GL_FOG);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    panViewPerspective(45);

    glPushMatrix();
    glLoadIdentity();
    //glTranslatef (0.f, 0.f, -15.f);							// Translate 6 Units Into The Screen
    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(rot.x, 1, 0, 0);
    glRotatef(rot.y, 0, 1, 0);
    glRotatef(rot.z, 0, 0, 1);
    dSphereEffect(pcolor, vcolor, base);
    glPopMatrix();

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(rot.x, 1, 0, 0);
    glRotatef(rot.y, 0, 1, 0);
    glRotatef(rot.z, 0, 0, 1);
    glRotatef(90, 0, 1, 0);
    dSphereEffect(pcolor, vcolor, base);
    glPopMatrix();

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(rot.x, 1, 0, 0);
    glRotatef(rot.y, 0, 1, 0);
    glRotatef(rot.z, 0, 0, 1);
    glRotatef(90, 1, 0, 0);
    dSphereEffect(pcolor, vcolor, base);
    glPopMatrix();

    glDisable(GL_FOG);
}

//Draws a sphere, with an exaggerated FOV
//Parameters :
// rot : rotation
// col : colour
void drawSfondo(const Vector3& rot, const rgb_a& col) {
    cubo->use();

    glPushMatrix();
    glLoadIdentity();

    panViewPerspective(168);

    glScalef(2, 2, 2);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDepthMask(GL_FALSE);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    cubo->use();
    glTranslatef(0, 0, 0);
    glRotatef(rot.x, 1, 0, 0);
    glRotatef(rot.y, 0, 1, 0);
    glRotatef(rot.z, 0, 0, 1);
    glColor4f(col.r, col.g, col.b, col.a);
    gluSphere(m_glqMyQuadratic, 1.3, 32, 32);
    glDepthMask(GL_TRUE);
    glPopMatrix();

    panViewPerspective(45);
}


//Draw a border on top/bottom of the screen
// bcol : colour used for the border side
// ccolor : colour used for the central side
// size : size of borders
void drawBorder(rgb_a bcol, rgb_a ccol, int size) {
    glPushMatrix();
    panViewOrtho();
    GLTexture::disable();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBegin(GL_QUADS);
    glColor4f(bcol.r, bcol.g, bcol.b, bcol.a);
    glVertex3i(0, HEIGHT, -1);
    glVertex3i(WIDTH, HEIGHT, -1);

    glColor4f(ccol.r, ccol.g, ccol.b, ccol.a);
    glVertex3i(WIDTH, HEIGHT - size, -1);
    glVertex3i(0, HEIGHT - size, -1);

    glColor4f(ccol.r, ccol.g, ccol.b, ccol.a);
    glVertex3i(0, size, -1);
    glVertex3i(WIDTH, size, -1);

    glColor4f(bcol.r, bcol.g, bcol.b, bcol.a);
    glVertex3i(WIDTH, 0, -1);
    glVertex3i(0, 0, -1);
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
void drawBlend(rgb_a col, int sx, int sy, int ex, int ey, GLenum modes, GLenum moded, GLTexture* tex) {

    if (col.a <= 0)
        return;

    panViewPerspective();

    glDepthMask(GL_FALSE);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    if (tex != nullptr) {
        glEnable(GL_TEXTURE_2D);
        tex->use();
    }

    glBlendFunc(modes, moded);
    glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_CULL_FACE);
    glDisable(GL_COLOR_MATERIAL);

    glPushMatrix();

    panViewOrtho();
    glBegin(GL_QUADS);
    glColor4f(col.r, col.g, col.b, col.a);
    glTexCoord2f(0.f, 0.f);
    glVertex3f((float)sx, (float)sy, -0.1f);

    glTexCoord2f(1.f, 0.f);
    glVertex3f((float)ex, (float)sy, -0.1f);

    glTexCoord2f(1.f, 1.f);
    glVertex3f((float)ex, (float)ey, -0.1f);

    glTexCoord2f(0.f, 1.f);
    glVertex3f((float)sx, (float)ey, -0.1f);
    glEnd();

    glPopMatrix();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);

}


void drawBlendBis(float t, float power, rgb_a col, int sx, int sy, int ex, int ey, int seg, float disp) {

    constexpr int MAXSEG = 20;
    if (seg >= MAXSEG) seg = MAXSEG - 1;
    static float posx[MAXSEG][MAXSEG];
    static float posy[MAXSEG][MAXSEG];
    static float module[MAXSEG][MAXSEG];
    if (col.a <= 0)
        return;

    panViewPerspective();

    glDepthMask(GL_FALSE);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_CULL_FACE);
    glDisable(GL_COLOR_MATERIAL);

    glPushMatrix();

    panViewOrtho();
    glBegin(GL_QUADS);

    glColor4f(col.r, col.g, col.b, col.a);
    float step = 1.f / seg;
    float dx = (ex - sx) * step;
    float dy = (ey - sy) * step;
    for (int i = 0; i <= seg; i++) {
        for (int j = 0; j <= seg; j++) {
            float a = vnoise(i * 0.25f, j * 0.25f, t);
            float b = vnoise(i * 0.25f, j * 0.25f, t + 1000);
            posx[i][j] = sx + dx * i + disp * powf(a, power) * ((a > 0) ? 1 : -1);
            posy[i][j] = sy + dy * j + disp * powf(b, power) * ((b > 0) ? 1 : -1);
            module[i][j] = col.a * (2 + a * a + b * b) / 4;
        }
    }
    for (int i = 0; i < seg; i++) {
        for (int j = 0; j < seg; j++) {
            glColor4f(col.r, col.g, col.b, module[i][j]);
            glTexCoord2f(step * i, step * j);
            glVertex3f(posx[i][j], posy[i][j], -0.1f);

            glColor4f(col.r, col.g, col.b, module[i + 1][j]);
            glTexCoord2f(step * (i + 1), step * j);
            glVertex3f(posx[i + 1][j], posy[i + 1][j], -0.1f);

            glColor4f(col.r, col.g, col.b, module[i + 1][j + 1]);
            glTexCoord2f(step * (i + 1), step * (j + 1));
            glVertex3f(posx[i + 1][j + 1], posy[i + 1][j + 1], -0.1f);

            glColor4f(col.r, col.g, col.b, module[i][j + 1]);
            glTexCoord2f(step * i, step * (j + 1));
            glVertex3f(posx[i][j + 1], posy[i][j + 1], -0.1f);
        }
    }
    glEnd();

    glPopMatrix();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
}

//Just draw some vertical JLSAC lines on the screen
void drawLines(float t, float alpha, int n) {
    glDepthMask(GL_FALSE);
    glPushMatrix();
    panViewPerspective();
    GLTexture::disable();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBegin(GL_LINES);
    glColor4f(1.f, 0.6f, 0.3f, alpha);
    for (int i = 0; i < n; i++) {
        float spd = vlattice(i, 1) * 10;
        float phs = vlattice(i, 2) * 10;
        glVertex3f(cosf(t / 2 * spd) * sinf(t / 2 + phs) * 5, -100, -10);
        glVertex3f(cosf(t / 2 * spd) * sinf(t / 2 + phs) * 5, 100, -10);
    }
    glEnd();

    glPopMatrix();
}

void dBugs(float t, bool calc, float xpos, float zpos, float scale, float alphazero, float xtrans, float ytrans, float ztrans) {
    glPushMatrix();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glTranslatef(xpos, 0, zpos);
    glScalef(scale, scale, scale);
    dNuts(t, calc, true, -0.3f, { 1.f, 1.f, 1.f, alphazero });  //just loads the vertices once
    glScalef(1.f, -1.f, 1.f);
    glTranslatef(0.f, 0.5f, 0.f);
    dNuts(t, false, true, -0.3f, { 1.f, 1.f, 1.f, 0.1f });  //just loads the vertices once
    glTranslatef(0.f, -0.5f, 0.f);
    glScalef(1.f, 0.001f, 1.f);
    glTranslatef(xtrans, ytrans, ztrans);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    dNuts(t, false, true, -0.3f, { 0.f, 0.f, 0.f, 0.05f });  //just loads the vertices once
    glPopMatrix();
}

void drawBugs(float t, rgb_a barcolor, Vector3 pos, Vector3 rot, Vector3 size, float nutpos, Vector3 eye) {
    glClearColor(0.3f, 0.2f, 0.1f, 0.5f);

    float fogColor[4] = { 0.3f, 0.2f, 0.1f, 0.f };

    panViewPerspective(45.f);
    gluLookAt((double)eye.x, (double)eye.y, (double)eye.z, 0, 0, -1, 0, 1, 0);
    // fog stuff
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, 0.01f);
    glFogf(GL_FOG_START, 6);
    glFogf(GL_FOG_END, 22);
    glHint(GL_FOG_HINT, GL_FASTEST);
    glEnable(GL_FOG);

    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(rot.x, 1, 0, 0);
    glRotatef(rot.y, 0, 1, 0);
    glRotatef(rot.z, 0, 0, 1);

    glEnable(GL_LIGHTING);
    GLTexture::disable();
    glEnable(GL_COLOR_MATERIAL);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glPushMatrix();
    glColor4fv((float*)&barcolor);
    dCube(size);
    glPopMatrix();

    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);


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
    gluLookAt((double)eye.x, (double)eye.y, (double)eye.z, 0, 0, -1, 0, 1, 0);
    glColor4f(1.f, 1.f, 1.f, 0.8f);
    glTranslatef(pos.x, pos.y + (size.y / 2.f), pos.z);
    glRotatef(rot.x, 1, 0, 0);
    glRotatef(rot.y + 90, 0, 1, 0);
    glRotatef(rot.z, 0, 0, 1);

    dBugs(t, true, nutpos - 1.9f, 1.3f, 0.6f, 0.9f, -0.5f, -20.f, -0.5f);
    dBugs(t, true, nutpos, 0.3f, 0.6f, 0.8f, -0.5f, -20.f, -0.5f);
    dBugs(t, false, nutpos + 1.6f, -1.6f, 0.5f, 0.8f, -0.7f, -1.f, -0.5f);
    dBugs(t, false, nutpos + 2.6f, 2.5f, 0.5f, 0.8f, -0.7f, -1.f, -0.5f);
    dBugs(t, false, nutpos + 3.5f, 0.5f, 0.6f, 0.8f, -0.7f, -1.f, -0.5f);
    dBugs(t, false, nutpos + 4.5f, -1.5f, 0.7f, 0.8f, -0.7f, -1.f, -0.5f);
    dBugs(t, false, nutpos + 6.1f, -0.5f, 0.5f, 0.8f, -0.7f, -1.f, -0.5f);
    dBugs(t, false, nutpos + 7.7f, -2.5f, 0.5f, 0.8f, -0.7f, -1.f, -0.5f);
    dBugs(t, false, nutpos + 8.1f, -0.3f, 0.5f, 0.8f, -0.7f, -1.f, -0.5f);
    dBugs(t, false, nutpos + 8.6f, 2.3f, 0.6f, 0.8f, -0.7f, -1.f, -0.5f);
    dBugs(t, false, nutpos + 9.1f, -2.3f, 0.6f, 0.8f, -0.7f, -1.f, -0.5f);
    dBugs(t, false, nutpos + 9.4f, 0.5f, 0.5f, 0.8f, -0.7f, -1.f, -0.5f);
    dBugs(t, false, nutpos + 9.6f, 1.5f, 0.6f, 0.8f, -0.7f, -1.f, -0.5f);

    glPopMatrix();
}

void drawCreditsBack(float t) {
    drawBlendBis(t / 1.454875f * 4       , 8, { 1.f, 1.f, 1.f, 0.250000f }, -5 * WIDTH / 32, -5 * HEIGHT / 32, 37 * WIDTH / 32, 37 * HEIGHT / 32, 20, std::min(20.f * WIDTH / 640, 20.f * HEIGHT / 480));
    drawBlendBis(t / 1.454875f * 4 - 0.2f, 4, { 1.f, 1.f, 1.f, 0.125000f }, -5 * WIDTH / 32, -5 * HEIGHT / 32, 37 * WIDTH / 32, 37 * HEIGHT / 32, 20, std::min(30.f * WIDTH / 640, 30.f * HEIGHT / 480));
    drawBlendBis(t / 1.454875f * 4 - 0.4f, 2, { 1.f, 1.f, 1.f, 0.062500f }, -5 * WIDTH / 32, -5 * HEIGHT / 32, 37 * WIDTH / 32, 37 * HEIGHT / 32, 20, std::min(40.f * WIDTH / 640, 40.f * HEIGHT / 480));
    drawBlendBis(t / 1.454875f * 4 - 0.8f, 1, { 1.f, 1.f, 1.f, 0.003125f }, -5 * WIDTH / 32, -5 * HEIGHT / 32, 37 * WIDTH / 32, 37 * HEIGHT / 32, 20, std::min(50.f * WIDTH / 640, 50.f * HEIGHT / 480));
}

void drawCredits(float t) {
    float sync = t / 1.454875f * 2 - 0.5f;
    //  float e = exp(6*(rsync-sync));
    texture1->use();

    drawCreditsBack(t);

    //scritte->use();
    glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    CoolPrint1(*FontArial, 20, sync, 0.f,  1.f,  3.f,  4.f, 0.34375f * WIDTH, (sync * 30 + 45) * HEIGHT / 480, 230.f * WIDTH / 640, 0.6f, 0.2f, "rio");
    CoolPrint1(*FontArial, 20, sync, 3.f,  4.f,  6.f,  7.f, 0.34375f * WIDTH, (sync * 30 + 45) * HEIGHT / 480, 200.f * WIDTH / 640, 0.6f, 0.f, "pan");
    CoolPrint1(*FontArial, 20, sync, 6.f,  7.f,  9.f, 10.f, 0.34375f * WIDTH, (sync * 30 + 45) * HEIGHT / 480, 160.f * WIDTH / 640, 0.6f, -0.1f, "dixan");
    CoolPrint1(*FontArial, 20, sync, 9.f, 10.f, 12.f, 13.f, 0.34375f * WIDTH, (sync * 30 + 45) * HEIGHT / 480, 170.f * WIDTH / 640, 0.6f, 0.f, "wiss");

    float t2 = skGetTime();
    glClearColor(0, 0, 0, 0.5);

    panViewPerspective();
    //enable stenciling writing
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
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
    glRotatef(sinf(t) * 45, 0, 0, 1);
    panViewPerspective(45.f);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    parts1.SetParticlesPerSec(200);
    parts1.SetSize(0.05f, 0.f);
    parts1.SetAngle(100.f);
    parts1.SetLife(3.f);
    parts1.SetSpeed(3.f);
    parts1.SetPosition({ 2.f, -13.f, -30.f });
    parts1.SetVelocity({ sinf(t2) / 1.5f, fabsf(cosf(t2)), 0.f });
    parts1.SetGravity({ 0.5f, 5, 0.f });
    parts1.SetSize(0.2f, 0.1f);
    parts1.StepOver(t2, 10);
    parts1.Draw();

    parts2.SetParticlesPerSec(200);
    parts2.SetSize(0.05f, 0.f);
    parts2.SetAngle(100.f);
    parts2.SetLife(3.f);
    parts2.SetSpeed(3.f);
    parts2.SetPosition({ 2.f, 0.f, -30.f });
    parts2.SetVelocity({ -sinf(t2) / 1.5f, -fabsf(cosf(t2)) * 3.f, 0.f });
    parts2.SetGravity({ 0.5f, -5.f, 0.f });
    parts2.SetSize(0.2f, 0.1f);
    parts2.StepOver(t2, 10);
    parts2.Draw();
    glPopMatrix();

    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 0, 0xffffffff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    panViewPerspective();
    glBindTexture(GL_TEXTURE_2D, logo);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    drawBlendBis(t / 1.45f, 1, { 1.f, 1.f, 1.f, 0.5f }, WIDTH / 2, 80 * HEIGHT / 480, WIDTH, HEIGHT - 80 * HEIGHT / 480, 10, std::min(50.f * WIDTH / 640, 50.f * HEIGHT / 480));

    glDisable(GL_STENCIL_TEST);


    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

}

void dPanLandscape(float t, float nstep, float len, float H) {
    constexpr int PANLANDSIZE = 50;
    float videostep = len / PANLANDSIZE;
    float heights[PANLANDSIZE + 1][PANLANDSIZE + 1];
    float center = -videostep * (PANLANDSIZE - 1) / 2.f;
    for (int i = 0; i <= PANLANDSIZE; i++) {
        float x = nstep * i;
        for (int j = 0; j <= PANLANDSIZE; j++) {
            float y = nstep * j;
            heights[i][j] = vnoise(x, y, t);
        }
    }

    glPushMatrix();
    glTranslatef(center, 0, center);
    glColor4f(1, 1, 1, 0.06f);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    float deltatex = 1 / (PANLANDSIZE - 1);
    for (int i = 0; i < PANLANDSIZE - 1; i += 2) {
        float x = videostep * i;
        float x1 = i * deltatex;
        for (int j = 0; j < PANLANDSIZE - 1; j += 2) {
            float z = videostep * j;
            float z1 = j * deltatex;

            glTexCoord2f(heights[i][j], x1);
            glVertex3f(x, heights[i][j] * H, z);
            glTexCoord2f(heights[i + 2][j], x1 + deltatex * 2);
            glVertex3f(x + videostep * 2, heights[i + 2][j] * H, z);
            glTexCoord2f(0, x1);
            glVertex3f(x + videostep * 2, -20, z);
            glTexCoord2f(0, x1 + deltatex * 2);
            glVertex3f(x, -20, z);

            glTexCoord2f(heights[i][j], z1);
            glVertex3f(x, heights[i][j] * H, z);
            glTexCoord2f(heights[i][j + 2], z1 + deltatex * 2);
            glVertex3f(x, heights[i][j + 2] * H, z + videostep * 2);
            glTexCoord2f(0, z1);
            glVertex3f(x, -20, z + videostep * 2);
            glTexCoord2f(0, z1 + deltatex * 2);
            glVertex3f(x, -20, z);
        }
        //    z = videostep*(PANLANDSIZE-1);
    }
    glEnd();
    glColor4f(1, 1, 1, 0.2f);

    GLTexture::disable();
    glBegin(GL_LINES);
    for (int i = 0; i < PANLANDSIZE - 1; i++) {
        float x = videostep * i;
        float z;
        for (int j = 0; j < PANLANDSIZE - 1; j++) {
            z = videostep * j;

            glVertex3f(x, heights[i][j] * H, z);
            glVertex3f(x + videostep, heights[i + 1][j] * H, z);
            glVertex3f(x, heights[i][j] * H, z);
            glVertex3f(x, heights[i][j + 1] * H, z + videostep);
        }
        z = videostep * (PANLANDSIZE - 1);
        glVertex3f(x, heights[i][PANLANDSIZE - 1] * H, z);
        glVertex3f(x + videostep, heights[i + 1][PANLANDSIZE - 1] * H, z);
        glVertex3f(z, heights[PANLANDSIZE - 1][i] * H, x);              // e' i, non j... coglione! x e z si scambiano...
        glVertex3f(z, heights[PANLANDSIZE - 1][i + 1] * H, x + videostep);
    }
    glEnd();
    glPopMatrix();
}

void drawPanLandscape(float t) {
    static float fogColor[4] = { 0, 0, 0, 0 };
    panViewPerspective();

    float len = 50;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    GLTexture::disable();
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogfv(GL_FOG_COLOR, fogColor);
    //	glFogf(GL_FOG_DENSITY, 0.01f );
    glFogf(GL_FOG_START, len / 8);//len/4);
    glFogf(GL_FOG_END, len / 4);
    glEnable(GL_FOG);
    glHint(GL_FOG_HINT, GL_FASTEST);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
    glRotatef(t * 10, 0, 1, 0);
    glRotatef(15, 1, 0, 0);
    glTranslatef(0, -3, 0);
    dPanLandscape(t, 0.2f, len / 2, 1.5f);
    glTranslatef(0, -0.1f, 0);
    dPanLandscape(t + 0.2f, 0.2f, len / 2, 1.6f);
    glDisable(GL_FOG);
}

void drawPanOverWiss(float t) {
    float t1 = t / 1.454875f;

    int stock1 = (int)floorf(t1);
    int stock2 = (int)floorf(t1 * 8);
    float randa[10];
    float t2 = HALFPI * t1;
    static const float wei[10] = { 0, 0, 0, 0, 0, 0, 0, 0.5, 0.5, 0.0 }; // da vicino nessuno e' normale!
    for (int i = 0; i < 10; i++) {
        randa[i] = wei[i] * vlattice(stock2, i) + (1 - wei[i]) * vlattice(stock1, i + 50);
    }
    drawWissEffect({ randa[0] * 4, randa[1] * 4, -18 + randa[2] * 4 }, { t * 30 + randa[3] * 6, sinf(t2) * 30, 0 }, { 1.f, 1.f, 1.f, 0.8f }, { 0.3f, 0.2f, 0.1f, fabsf(sinf(t2 + randa[8] * 3)) / 4.f }, true);
    drawWissEffect({ randa[4] * 4, randa[5] * 4, -12 + randa[6] * 4 }, { sinf(t2) * 30, t * 30 + randa[7] * 6, 0 }, { 1.f, 1.f, 1.f, 0.8f }, { 0.3f, 0.2f, 0.1f, fabsf(sinf(t2 + randa[9] * 3)) / 4.f }, true);
}

/*##########################################################*/
/*Implementation											*/
/*##########################################################*/

void ScenaRewind(int orderr)
{
    float speed = 8.f;
    float mytime = (skGetTime() - timebase[15]) * speed;
    float tr;

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    float durata = (timebase[15 - orderr] - timebase[14 - orderr]);
    tr = (skGetTime() - timebase[15] - (1.4548f * orderr)) / 1.4548f * durata;
    float tr2 = (orderr + 1) * 1.4548f * speed - mytime;

    CoolPrint1(*FontArial, 50, tr, 0, 1.f, 5.5f, 6.5f, 0.5f * WIDTH, 0.8333333f * HEIGHT, (50 - 15 * (1 - 1.f / (1.1f + sinf(tr * 0.8f)))) * WIDTH / 640, 0.6f, 0, "from a close sight nobody is normal");

    panViewPerspective(45);
    drawLines(tr, 0.5, 30);
    glDisable(GL_FOG);

    //coso energetico
    if (orderr == 0) {
        int pxx = std::max(0, (int)floor(tr / 1.4548f));
        int val = (pxx < 8) ? twirlstable[pxx] : 1;

        glDisable(GL_FOG);
        PrepareRenderToTexture((100.f - (sinf(mytime) * 45.f)), frame2->getSize());
        drawHelix(mytime * 300, 2.f, 3.5f, val, 30, GL_FILL, { 1, 0.7f, 0, 0 }, { 1, 0.7f, 0, 0.9f });
        drawHelix(mytime * 300, 2.f, 3.5f, val, 30, GL_LINE, { 1, 0.6f, 0, 0 }, { 1, 0.6f, 0, 0.9f });
        DoRenderToTexture(frame2);
        donerendertotexture = false;

        drawLines(mytime, 0.4f, 20);
        panViewPerspective();

        drawTexture(frame2, 1, 1, true, 0.3f, 12, { 1.f, 1.f, 1.f, 0.6f }, true);
        drawTexture(frame2, 1, 1, true, 0.4f, 15, { 1.f, 1.f, 1.f, 0.6f }, true);
        drawTexture(frame2, 1, 1, true, 0.5f, 18, { 1.f, 1.f, 1.f, 0.6f }, true);
        drawTexture(frame2, 1, 1, true, 0.6f, 21, { 0.8f, 0.8f, 0.8f, 0.6f }, true);
        drawTexture(frame2, 1, 1, true, 0.7f, 24, { 1.f, 1.f, 1.f, 0.6f }, true);
        drawTexture(frame2, 1, 1, true, 0.8f, 27, { 0.7f, 0.6f, 0.6f, 0.6f }, true);
        drawTexture(frame2, 1, 1, true, 0.9f, 30, { 1.f, 0.8f, 0.8f, 0.6f }, true);
        drawTexture(frame2, 1, 1, true, 1.f, 33, { 0.5f, 0.4f, 0.3f, 0.6f }, true);

        drawTexture(frame2, 1, 1, true, 1.f, 133, { 0.5f, 0.4f, 0.3f, 0.6f }, true);
        panViewOrtho();
    }

    //peli
    if (orderr == 1) {
        //scena 13 originale
        glDisable(GL_FOG);
        PrepareRenderToTexture(45, frame2->getSize()); //(100.f - (sin(mytime) * 45.f))
        glTranslatef(0, 0, -10);
        glRotatef(-sinf(mytime / 10) * 180.f, 1.f, 0.f, 0.f);
        glRotatef(-sinf(mytime / 10) * 45.f, 0.f, 1.f, 0.f);
        glRotatef(-sinf(mytime / 10) * 45.f, 0.f, 0.f, 1.f);
        dCylinder(0.2f, 24, 12, 5, sinf(mytime), cosf(mytime * 2) * 2, sinf(cosf(mytime)), mytime, true, { 0.9f, 0.9f, 0.9f, 0.f }, { 1.f, 1.f, 1.f, 0.7f }, 15);
        DoRenderToTexture(frame2);
        donerendertotexture = false;

        panViewPerspective();
        drawTexture(tex, 1, 1, true, mytime / 2, 12, { 0.7f, 0.7f, 0.7f, 0.8f }, false);

        drawTexture(frame2, 1, 1, true, 0.5f, 12, { 1.f, 1.f, 1.f, 0.8f }, true);
        drawTexture(frame2, 1, 1, true, 0.6f, 15, { 1.f, 1.f, 1.f, 0.8f }, true);
        drawTexture(frame2, 1, 1, true, 0.7f, 18, { 1.f, 1.f, 1.f, 0.8f }, true);
        drawTexture(frame2, 1, 1, true, 0.8f, 21, { 1.f, 1.f, 1.f, 0.8f }, true);
        drawTexture(frame2, 1, 1, true, 0.9f, 24, { 1.f, 1.f, 1.f, 0.8f }, true);
        drawTexture(frame2, 1, 1, true, 1.f, 27, { 1.f, 1.f, 1.f, 0.8f }, true);
        drawTexture(frame2, 1, 1, true, 1.f, 30, { 1.f, 1.f, 1.f, 0.8f }, true);
        drawTexture(frame2, 1, 1, true, 1.f, 33, { 1.f, 1.f, 1.f, 0.8f }, true);
    }

    //pan terreno
    if (orderr == 2) {
        drawPanLandscape(timebase[11] - tr);
    }

    //credits
    if (orderr == 3) {
        glDisable(GL_DEPTH_TEST);
        drawCredits(tr2);
    }

    //roba wiss
    if (orderr == 4) {
        drawLines(tr, 0.1f, 50);
        float t1 = tr / 1.454875f;

        int stock2 = (int)floorf(t1 * 8);

        if (stock2 < 52) {
            drawPanOverWiss(tr);
            panViewOrtho();
            scritte->use();
            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
            GLTexture::disable();
        } else {
            float t2 = HALFPI * t1;
            drawWissEffect({ 0.f, 0.f, -18.f + (stock2 - 52.f) }, { tr * 30.f, sinf(t2) * 30.f, 0.f }, { 1.f, 1.f, 1.f, 0.8f }, { 0.3f, 0.2f, 0.1f, fabsf(sinf(t2 * 5)) / 4.f }, true);
            glColor4f(1, 0.9f, 0.8f, (stock2 - 52.f) * 0.125f);
            panViewOrtho();
            glBegin(GL_QUADS);
            glVertex2i(0, 0);
            glVertex2i(WIDTH, 0);
            glVertex2i(WIDTH, HEIGHT);
            glVertex2i(0, HEIGHT);
            glEnd();
        }
    }

    //go
    if (orderr == 5) { // ma si, anche senzuncazzo sta bene...
    }

    //fade
    if (orderr == 6) {
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
        glEnable(GL_BLEND);
        panViewOrtho();
        GLTexture::disable();
    }

    //tubo peloso
    if (orderr == 7) {
        panViewPerspective(45);
        drawTubo(6, tr, { 0.9f, 0.9f, 0.9f, 0.1f }, { 0.5, 0.25, 0.0625, 0.5f });
    }

    //palline
    if (orderr == 8) {
        drawTexture(frame, 1.f, 1.f, true, tr, 80, { 1.f, 1.f, 1.f, 0.6f }, true);
        drawTexture(frame, 1.f, 1.f, false, tr, 80, { 1.f, 1.f, 1.f, 0.6f }, true);
        drawNuts(tr);
        drawWissEffect({ 3.f, 3.f, -4.f }, { mytime, mytime, 0.f }, { 1.f, 1.f, 1.f, 0.8f }, { 0.3f, 0.2f, 0.1f, fabsf(sinf(mytime)) / 4.f }, false);
    }

    //tubo peloso
    if (orderr == 9) {
        panViewPerspective(45);
        drawTubo(10, tr, { 0.9f, 0.9f, 0.9f, 0.1f }, { 0.5, 0.25, 0.0625, 0.5f });
    }

    //palline
    if (orderr == 10) {
        drawTexture(frame, 1.f, 1.f, true, tr, 80, { 1.f, 1.f, 1.f, 0.6f }, true);
        drawTexture(frame, 1.f, 1.f, false, tr, 80, { 1.f, 1.f, 1.f, 0.6f }, true);
        drawNuts(tr);
        drawWissEffect({ 3.f, 3.f, -4.f }, { mytime, mytime, 0.f }, { 1.f, 1.f, 1.f, 0.8f }, { 0.3f, 0.2f, 0.1f, fabsf(sinf(mytime)) / 4.f }, false);
    }

    //sfondo vuoto + texture toroide
    if (orderr == 11) {
        drawTexture(frame, 1.f, 1.f, true, tr, 80, { 1.f, 1.f, 1.f, 0.6f }, true);
        drawTexture(frame, 1.f, 1.f, false, tr, 80, { 1.f, 1.f, 1.f, 0.6f }, true);
        drawWissEffect({ 3.f, 3.f, -4.f }, { mytime, mytime, 0.f }, { 1.f, 1.f, 1.f, 0.8f }, { 0.3f, 0.2f, 0.1f, fabsf(sinf(mytime)) / 4.f }, false);
    }

    //flash + toroide
    if (orderr == 12) {
        drawSfondo({ sinf(tr) * 90.f, 0.f, sinf(tr * 2.f) * 45.f }, { 0.5, 0.5, 1.0, 1 });
        glDisable(GL_CULL_FACE);
        panViewPerspective(190.f - ((tr / durata) * 70.f));
        drawToroide(orderr, tr, mytime);
        float val = sinf(mytime) * 2.5f;
        glPushMatrix();
        glTranslatef(val, 0, 0);
        drawToroide(orderr, tr + 50, mytime + 1);
        glTranslatef(-2 * val, 0, 0);
        drawToroide(orderr, tr + 50, mytime + 2);
        glPopMatrix();
    }

    //toroide
    if (orderr >= 13) {
        drawSfondo({ sinf(tr) * 90.f, 0.f, sinf(tr * 2.f) * 45.f }, { 0.5, 0.5, 1.0, 1 });
        glDisable(GL_CULL_FACE);
        panViewPerspective(110.f - ((tr / durata) * 70.f));
        drawToroide(orderr, tr, mytime);
        float val = sinf(mytime) * 2.5f;
        glPushMatrix();
        glTranslatef(val, 0, 0);
        drawToroide(orderr, tr + 50, mytime + 1);
        glTranslatef(-2 * val, 0, 0);
        drawToroide(orderr, tr + 50, mytime + 2);
        glPopMatrix();
    }
}

void Scena(float mytime, int order) {
    float t = mytime - timebase[order];

    //static GLTexture* frame =  perlin(8, 132, 0.3f, 0.7f, true);

  //################### rIO stuff do not even try to understand that  :D
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    tex2->use();
    //merdaccia per renderare nella texture i resti del toroide
    //SHIT SHIT SHIT!!! used to render once in the texture the rest of first scene torus
    if ((order == 2) && (donerendertotexture == false)) {
        PrepareRenderToTexture((180.f - (mytime * 15.f)), frame->getSize());
        //BEWARE oof matrices, do not use other drawing calls, or pay attention to
        //matrices they set, in this part no matrices ops should be done!!
        //attenzione alla merda che fanno le altre scene NON USARLE PER ORA!
        //drawSfondo(order,t);
    //      panViewPerspective(180.f - (skGetTime() * 15.f));
        drawToroide(order, t, mytime);
        float val = sinf(mytime) * 2.5f;
        glPushMatrix();
        glTranslatef(val, 0, 0);
        drawToroide(order, t + 50, mytime + 1);
        glTranslatef(-2 * val, 0, 0);
        drawToroide(order, t + 50, mytime + 2);
        glPopMatrix();
        DoRenderToTexture(frame);
    }

    if ((order == 7) && (donerendertotexture == false)) {
        PrepareRenderToTexture(45, frame2->getSize());
        //BEWARE oof matrices, do not use other drawing calls, or pay attention to
        //matrices they set, in this part no matrices ops should be done!!
        //attenzione alla merda che fanno le altre scene NON USARLE PER ORA!
        //drawSfondo(order,t);
        drawTubo(order, timebase[7] - timebase[6], { 0.9f, 0.9f, 0.9f, 0.1f }, { 0.5, 0.25, 0.0625, 0.5f });
        DoRenderToTexture(frame2);
    }

    if (order == 0) {
        drawSfondo({ sinf(t) * 90.f, 0.f, sinf(t * 2.f) * 45.f }, { 0.5, 0.5, 1.0, 1 });
    }

    if (order == 1) {
        GLTexture::disable();
        drawBlend({ 1.f, 1.f, 1.f, 1.f - (t / 2) }, 0, 0, WIDTH, HEIGHT, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, nullptr);
    }

    if (order < 4) { //(order == 0) &&
        panViewPerspective(180.f - (skGetTime() * 15.f));
        drawToroide(order, t, mytime);
        float val = sinf(mytime) * 2.5f;
        glPushMatrix();
        glTranslatef(val, 0, 0);
        drawToroide(order, t + 50, mytime + 1);
        glTranslatef(-2 * val, 0, 0);
        drawToroide(order, t + 50, mytime + 2);
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
            CoolPrint1(*FontArial, 50, t, 0.f, 1.5f, 8.f, 10.f, 0.375f*WIDTH, 0.21875f*HEIGHT, 0.09375f*WIDTH, 0.6f, -0.2f, "reality is that which");
            CoolPrint1(*FontArial, 50, t, 1.5f, 3.f, 8.f, 10.f, 0.625f*WIDTH, 0.78125f*HEIGHT, 0.09375f*WIDTH, 0.6f, -0.2f, "doesn't go away");
            CoolPrint1(*FontArial, 50, 10 - t, -1.f, 1.f, 5.f, 7.f, 0.5f*WIDTH, 0.5f*HEIGHT, 0.06250f*WIDTH, 0.6f, 0.2f, "when you stop believing in it");
            //        CoolPrint1(*FontArial, 50,    t, 6.f, 8.f, 9.f, 10.f, 480, 400, 60, 0.6f, -0.2f, "Philip K. Dick");
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
            GLTexture::disable();
        }
    }


    if (order == 2) { // && ((0.4f - (mytime / 100.f)) > 0))
        panViewPerspective(180.f - (mytime * 15.f));
        drawToroide(order, t, mytime);
        drawTexture(frame, 1.f, 1.f, true, t, 80, { 1.f, 1.f, 1.f, fabsf(sinf(t * 1.3f)) }, true);
        drawTexture(frame, 1.1f, 1.f, false, t, 80, { 1.f, 1.f, 1.f, fabsf(sinf(t * 1.2f)) }, true);
        drawTexture(frame, 1.2f, 1.f, false, t, 80, { 1.f, 1.f, 1.f, fabsf(sinf(t * 1.1f)) }, true);
        drawTexture(frame, 1.3f, 1.f, false, t, 80, { 1.f, 1.f, 1.f, fabsf(sinf(t * 1.f)) }, true);
        drawWissEffect({ 3.f, 3.f, -4.f }, { mytime, mytime, 0.f }, { 1.f, 1.f, 1.f, 0.8f }, { 0.3f, 0.2f, 0.1f, fabsf(sinf(mytime)) / 4.f }, false);

        glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
        glEnable(GL_BLEND);
        panViewOrtho();
        scritte->use();
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        CoolPrint1(*FontArial, 50, t, 0.f, 1.5f, 1.5f, 3.f, 140 * WIDTH / 640,  80 * HEIGHT / 480, 60 * WIDTH / 480, 0.6f, 0.5f, "clean");
        CoolPrint1(*FontArial, 50, t, 1.5f, 3.f, 3.f, 4.5f, 500 * WIDTH / 640, 400 * HEIGHT / 480, 40 * WIDTH / 480, 0.8f, -0.1f, "perceptions");
        glColor3f(1, 1, 1);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        GLTexture::disable();
        drawLines(t, 0.3f, 10);
    }

    if ((order == 3) || (order == 5)) {
        drawTexture(frame, 1.f, 1.f, true, t, 80, { 1.f, 1.f, 1.f, 0.6f }, true);
        drawTexture(frame, 1.f, 1.f, false, t, 80, { 1.f, 1.f, 1.f, 0.6f }, true);
        drawNuts(t);
        drawWissEffect({ 3.f, 3.f, -4.f }, { mytime, mytime, 0.f }, { 1.f, 1.f, 1.f, 0.8f }, { 0.3f, 0.2f, 0.1f, fabsf(sinf(mytime)) / 4.f }, false);

        glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
        glEnable(GL_BLEND);
        panViewOrtho();
        scritte->use();
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        if (order == 3) {
            CoolPrint1(*FontArial, 50, t, -1.5f, 0.f, 1.f, 2.5f, 500 * WIDTH / 640, 400 * HEIGHT / 480, 40 * WIDTH / 480, 0.8f, -0.1f, "perceptions");
        } else {
            CoolPrint1(*FontArial, 50, t, 0, 1.5f, 1.5f, 3.f, 140 * WIDTH / 640, 80 * HEIGHT / 480, 60 * WIDTH / 480, 0.6f, 0.5, "clear");
            CoolPrint1(*FontArial, 50, t, 1.5f, 3.f, 3.f, 4.5f, 500 * WIDTH / 640, 400 * HEIGHT / 480, 40 * WIDTH / 480, 0.8f, -0.1f, "illusions");
        }
        glColor3f(1, 1, 1);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        GLTexture::disable();
        drawLines(t, 0.3f, 10);
    }

    if ((order == 4) || (order == 6)) {
        drawSfondo({ sinf(t) * 90.f, 0, sinf(t * 2) * 45.f }, { 0.5f, 0.5f, 1.f, 1.f });
        panViewPerspective(45);
        drawTubo(order, t, { 0.9f, 0.9f, 0.9f, 0.1f }, { 0.5f, 0.25f, 0.0625f, 0.5f });
        donerendertotexture = false;

        glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
        glEnable(GL_BLEND);
        panViewOrtho();
        scritte->use();
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        if (order == 4) {
            CoolPrint1(*FontArial, 50, t, 0, 1.5f, 1.5f, 3.f, 140.f * WIDTH / 640, 80.f * HEIGHT / 480, 60.f * WIDTH / 480, 0.6f, 0.5, "fuzzy");
            CoolPrint1(*FontArial, 50, t, 1.5f, 3.f, 3.f, 4.5f, 500.f * WIDTH / 640, 400.f * HEIGHT / 480, 40.f * WIDTH / 480, 0.8f, -0.1f, "dimensions");
        } else {
            CoolPrint1(*FontArial, 50, t, 0, 1.5f, 1.5f, 3.f, 140.f * WIDTH / 640, 80.f * HEIGHT / 480, 60.f * WIDTH / 480, 0.6f, 0.5, "furry");
            CoolPrint1(*FontArial, 50, t, 1.5f, 3.f, 3.f, 4.5f, 500.f * WIDTH / 640, 400.f * HEIGHT / 480, 40.f * WIDTH / 480, 0.8f, -0.1f, "onions");
        }
        glColor3f(1, 1, 1);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        GLTexture::disable();
    }

    if (order == 7) {
        if ((t * 1.6f) < 1.f) {
            panViewPerspective(45);
            drawTubo(order, mytime, { 0.9f, 0.9f, 0.9f, 0.1f - (t * 2) }, { 0.5f - (t * 2), 0.25f - (t * 2), 0.0625f - (t * 2), 0.5f - (t * 4) });

            drawBlend({ 1.f, 1.f, 1.f, 1.f - (t * 2.f) }, 0, (int)-(t * 150.6f), WIDTH, (int)(HEIGHT + (t * 150.6f)), GL_SRC_ALPHA, GL_ONE, frame2);
            drawBlend({ 1.f, 1.f, 1.f, 1.f - (t * 2.f) }, 0, (int)-(t * 150.6f), WIDTH, (int)(HEIGHT + (t * 150.6f)), GL_SRC_ALPHA, GL_ONE, frame2);
            drawBlend({ 1.f, 1.f, 1.f, 1.f - (t * 2.f) }, 0, (int)-(t * 150.6f), WIDTH, (int)(HEIGHT + (t * 150.6f)), GL_SRC_ALPHA, GL_ONE, frame2);
        }
    }

    if (order == 8) {
        glColor4f(1.f, 1.f, 1.f, 0.6f);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glEnable(GL_BLEND);
        panViewOrtho();
        scritte->use();
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        CoolPrint1(*FontArial, 50, t, 0, 0.1f, 5.5f, 6.5f, WIDTH / 2, HEIGHT / 2 - 20, (200 + 50 * sinf(t)) * WIDTH / 640, 0.5f, -0.25f * sinf(t), "GO");
        CoolPrint1(*FontArial, 50, t, 0, 0.1f, 5.5f, 6.5f, 500 * WIDTH / 640, 400 * HEIGHT / 480, 50 * WIDTH / 480, 1.5f, -0.25f * sinf(t), "where?");
        glColor3f(1, 1, 1);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        GLTexture::disable();
    }

    if (order == 9) {
        drawLines(t, 0.1f, 50);
        float t1 = t / 1.454875f;

        int stock2 = (int)floorf(t1 * 8);

        if (stock2 < 52) {
            drawPanOverWiss(t);
            panViewOrtho();
            scritte->use();
            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
            CoolPrint1(*FontArial, 50, t, 0, 1.f, 5.5f, 6.5f, 0.5 * WIDTH, 400 * HEIGHT / 480, (50 - 15 * (1 - 1.f / (1.1f + sinf(t * 0.8f)))) * WIDTH / 640, 0.6f, 0, "from a close sight nobody is normal");
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
            GLTexture::disable();
        } else {
            float t2 = HALFPI * t1;
            drawWissEffect({ 0.f, 0.f, -18.f + (stock2 - 52.f) }, { t * 30.f, sinf(t2) * 30.f, 0.f }, { 1.f, 1.f, 1.f, 0.8f }, { 0.3f, 0.2f, 0.1f, fabsf(sinf(t2 * 5)) / 4.f }, true);
            glColor4f(1, 0.9f, 0.8f, (stock2 - 52) * 0.125f);
            panViewOrtho();
            glBegin(GL_QUADS);
            glVertex2i(0, 0);
            glVertex2i(WIDTH, 0);
            glVertex2i(WIDTH, HEIGHT);
            glVertex2i(0, HEIGHT);
            glEnd();
        }

    }

    if (order == 10) {
        drawCredits(t);
    }

    if (order == 11) {
        drawPanLandscape(mytime);
        glDisable(GL_FOG);
        panViewOrtho();
        scritte->use();
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        CoolPrint1(*FontArial, 50, t, 1.0f, 2.f,  5.8f,   7.f, 0.5f * WIDTH, 80 * HEIGHT / 480, (100 + 25 * sinf(t)) * WIDTH / 640, 0.7f, -0.25f * sinf(t), "look around");
        CoolPrint1(*FontArial, 50, t, 5.8f, 7.f, 10.6f, 11.6f, 0.5f * WIDTH, 80 * HEIGHT / 480, (100 + 25 * sinf(t)) * WIDTH / 640, 0.7f, -0.25f * sinf(t), "look inside");
        glColor3f(1, 1, 1);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        GLTexture::disable();
    }

    //peli
    if (order == 12) {
        glDisable(GL_FOG);
        PrepareRenderToTexture(45, frame2->getSize()); //(100.f - (sin(mytime) * 45.f))
        glTranslatef(0, 0, -10);
        glRotatef(sinf(mytime / 10) * 180.f, 1.f, 0.f, 0.f);
        glRotatef(sinf(mytime / 10) * 45.f, 0.f, 1.f, 0.f);
        glRotatef(sinf(mytime / 10) * 45.f, 0.f, 0.f, 1.f);
        dCylinder(0.2f, 24, 12, 5, sinf(mytime), cosf(mytime * 2) * 2, sinf(cosf(mytime)), mytime, true, { 0.9f, 0.9f, 0.9f, 0.f }, { 1.0, 1.0, 1.0, 0.7f }, 15);
        DoRenderToTexture(frame2);
        donerendertotexture = false;

        panViewPerspective();

        drawTexture(tex, 1, 1, true, mytime / 2, 12, { 0.7f, 0.7f, 0.7f, 0.8f }, false);


        drawTexture(frame2, 1, 1, true, 0.5f, 12, { 1.f, 1.f, 1.f, 0.8f }, true);
        drawTexture(frame2, 1, 1, true, 0.6f, 15, { 1.f, 1.f, 1.f, 0.8f }, true);
        drawTexture(frame2, 1, 1, true, 0.7f, 18, { 1.f, 1.f, 1.f, 0.8f }, true);
        drawTexture(frame2, 1, 1, true, 0.8f, 21, { 1.f, 1.f, 1.f, 0.8f }, true);
        drawTexture(frame2, 1, 1, true, 0.9f, 24, { 1.f, 1.f, 1.f, 0.8f }, true);
        drawTexture(frame2, 1, 1, true, 1.f, 27, { 1.f, 1.f, 1.f, 0.8f }, true);
        drawTexture(frame2, 1, 1, true, 1.f, 30, { 1.f, 1.f, 1.f, 0.8f }, true);
        drawTexture(frame2, 1, 1, true, 1.f, 33, { 1.f, 1.f, 1.f, 0.8f }, true);
        //SUKA FINE MODIFICHE

        glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
        glEnable(GL_BLEND);
        panViewOrtho();
        scritte->use();
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        CoolPrint1(*FontArial, 50, t, 0.f, 1.f, 11.4f, 11.6f, 0.5 * WIDTH,  60 * HEIGHT / 480, (50 + 5 * sinf(t)) * WIDTH / 640, 0.5f, 0, "do you have the");
        CoolPrint1(*FontArial, 50, t, 1.f, 2.f, 11.4f, 11.6f, 0.5 * WIDTH, 400 * HEIGHT / 480, 120 * WIDTH / 640, 1.f, -0.25f * sinf(t), "POWER");
    }

    //flusso energetico lamadonna
    if (order == 13) {
        int pxx = std::max(0, (int)floor(t / 1.4548f));
        int val = (pxx < 8) ? twirlstable[pxx] : 1;

        glDisable(GL_FOG);
        PrepareRenderToTexture((100.f - (sinf(mytime) * 45.f)), frame2->getSize());
        drawHelix(mytime * -300, 2.f, 3.5f, val, 30, GL_FILL, { 1.f, 0.7f, 0.f, 0.f }, { 1.f, 0.7f, 0.f, 0.9f });
        drawHelix(mytime * -300, 2.f, 3.5f, val, 30, GL_LINE, { 1.f, 0.6f, 0.f, 0.f }, { 1.f, 0.6f, 0.f, 0.9f });
        DoRenderToTexture(frame2);
        donerendertotexture = false;

        drawLines(mytime, 0.4f, 20);
        panViewPerspective();

        drawTexture(frame2, 1, 1, true, 0.3f, 12, { 1.f, 1.f, 1.f, 0.6f }, true);
        drawTexture(frame2, 1, 1, true, 0.4f, 15, { 1.f, 1.f, 1.f, 0.6f }, true);
        drawTexture(frame2, 1, 1, true, 0.5f, 18, { 1.f, 1.f, 1.f, 0.6f }, true);
        drawTexture(frame2, 1, 1, true, 0.6f, 21, { 0.8f, 0.8f, 0.8f, 0.6f }, true);
        drawTexture(frame2, 1, 1, true, 0.7f, 24, { 1.f, 1.f, 1.f, 0.6f }, true);
        drawTexture(frame2, 1, 1, true, 0.8f, 27, { 0.7f, 0.6f, 0.6f, 0.6f }, true);
        drawTexture(frame2, 1, 1, true, 0.9f, 30, { 1.f, 0.8f, 0.8f, 0.6f }, true);
        drawTexture(frame2, 1, 1, true, 1.f, 33, { 0.5f, 0.4f, 0.3f, 0.6f }, true);

        drawTexture(frame2, 1, 1, true, 1.f, 133, { 0.5f, 0.4f, 0.3f, 0.6f }, true);
        panViewOrtho();

        scritte->use();
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        CoolPrint1(*FontArial, 50, t, 0, 1.f, 5.6f, 5.8f, 0.5f * WIDTH, 100 * HEIGHT / 480, (50 + 5 * sinf(t)) * WIDTH / 640, 0.5f, 0, "to make your  d r e a m s ");
        CoolPrint1(*FontArial, 50, t, 5.7f, 5.9f, 11.4f, 11.6f, 0.5f * WIDTH, 100 * HEIGHT / 480, (50 + 5 * sinf(t)) * WIDTH / 640, 0.5f, 0, "to make your nightmares");
        CoolPrint1(*FontArial, 50, t, 1.f, 2.f, 11.4f, 11.6f, 0.5f * WIDTH, 300 * HEIGHT / 480, 120 * WIDTH / 640, 1.f, -0.25f * sinf(t), "REAL?");
        glColor3f(1, 1, 1);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        GLTexture::disable();

    }


    //SUKA ULTIMA SCENA DEI CAZZIBBUFI CHE CAMMINANO
    //bugs
    if (order == 14) {
        Vector3 occhio;
        if (t < (1.4548f * 4)) {
            occhio = Vector3{};
        } else if ((t >= (1.4548f * 4)) && (t < (1.4548f * 5))) {
            float ang = ((t - (1.4548f * 4)) / 1.4548f) * PI;
            occhio = Vector3{ -sinf(ang) * 15.f, (t - (1.4548f * 4.f)) * 4.f, -(t - (1.4548f * 4.f)) * 15.f };
        } else if (t >= (1.4548f * 5.f)) {
            occhio = Vector3{ 0, 1.4548f * 4.f, -1.4548f * 15.f };
        }

        glEnable(GL_LIGHT0);
        drawBugs(mytime - timebase[14], { 1, 1, 1, 1 }, { 0.f, -1.f, -8.f }, { 15.f, 15.f, 0.f }, { 6.f, 1.f, 30.f }, -(t / 2.8f), occhio);

        glDisable(GL_LIGHT0);
        glDisable(GL_FOG);

        panViewOrtho();

        scritte->use();
        glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

        CoolPrint1(*FontArial, 50, t, 0, 1, 10.5f, 11.5f, 370 * WIDTH / 640, 0.25f * HEIGHT, 70 * WIDTH / 640, 0.7f, 0, "Join us!");
        glColor3f(1, 1, 1);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        GLTexture::disable();

        drawLines(t, 0.3f, 20);
    }

    if (order == 15) {
        glClearColor(0.f, 0.f, 0.f, 0.5f);
        float sync = t / 1.454875f * 2 - 0.5f;
        float rsync = (sync > 0) ? floorf(sync) : -0.5f;
        //    float e = exp(6*(rsync-sync));
        ScenaRewind((int)rsync);
        //flash
        panViewOrtho();

        scritte->use();
        glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

        static const char* nomi[12] = { "haujobb", "", "mfx", "farbrausch", "mewlers", "vantage", "foobug", "purple", "kolor", "hirmu", "calodox" };
        // quello vuoto e' vuoto perche' non si vede
        for (int i = 0; i < 12; i++) {
            CoolPrint1(*FontArial, 50, sync, i - 0.1f, (float)i, i + 1.f, i + 1.1f, (420.f - 160.f * (i % 2)) * WIDTH / 640, (i * 30.f + 100.f) * HEIGHT / 480, 110.f * WIDTH / 640, 0.7f, 0, nomi[i]);
        }


        glColor3f(1, 1, 1);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        GLTexture::disable();
    }


    //################### PaN stuff

    if ((order == 9) || (order == 10) || (order == 15) || (order == 12) || (order == 13)) {
        //flash
        float sync = t / 1.454875f * 2 - 0.5f;
        float rsync = (sync > 0) ? floorf(sync) : -0.5f;
        float e = expf(6 * (rsync - sync));

        if ((order != 15) || (rsync <= 13)) {
            glClearColor(0, 0, 0, 0.5);
            texture2->use();
            drawBlendBis(rsync, 1, { 4.8f, 4.5f, 4.5f, 1.33f * e }, -5 * WIDTH / 32, -5 * HEIGHT / 32, 37 * WIDTH / 32, 37 * HEIGHT / 32, 20, std::min(50.f * WIDTH / 640, 50.f * HEIGHT / 480));
            drawBlendBis(rsync, 1, { 4.5f, 4.8f, 4.5f, 1.33f * e }, -5 * WIDTH / 32, -5 * HEIGHT / 32, 37 * WIDTH / 32, 37 * HEIGHT / 32, 20, std::min(50.f * WIDTH / 640, 50.f * HEIGHT / 480));
            drawBlendBis(rsync, 1, { 4.5f, 4.5f, 4.8f, 1.33f * e }, -5 * WIDTH / 32, -5 * HEIGHT / 32, 37 * WIDTH / 32, 37 * HEIGHT / 32, 20, std::min(50.f * WIDTH / 640, 50.f * HEIGHT / 480));
        }
        if (order == 9) { // solo sui wisscosi!
            if (sync > 13) { // total in sync 14.4, in time 10.471
                glColor4f(0, 0, 0, (t - 12.5f) / 1.9f); // 10.471
                glBegin(GL_QUADS);
                glVertex2i(0, 0);
                glVertex2i(WIDTH, 0);
                glVertex2i(WIDTH, HEIGHT);
                glVertex2i(0, HEIGHT);
                glEnd();
            }
        }
    }


    //stoppare la musica stocazzo.
    if (order == 16) {
        panViewOrtho();

        scritte->use();
        glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        CoolPrint1(*FontArial, 50, t, -0.06f, 0.f, 5.6f, 5.8f, 270 * WIDTH / 640, 0.25f * HEIGHT, 120 * WIDTH / 640, 0.7f, 0, "P");
        CoolPrint1(*FontArial, 50, t, 0.20f, 0.26f, 5.6f, 5.8f, 370 * WIDTH / 640, 0.25f * HEIGHT, 120 * WIDTH / 640, 0.7f, 0, "K");
        CoolPrint1(*FontArial, 50, t, 0.47f, 0.53f, 11.4f, 11.6f, 0.5f * WIDTH, 0.5f * HEIGHT, 120 * WIDTH / 640, 0.7f, 0, "IS");
        CoolPrint1(*FontArial, 50, t, 0.73f, 0.79f, 11.4f, 11.6f, 0.5f * WIDTH, 0.75f * HEIGHT, (120 + 40 * sinf((t - 0.73f) * 3)) * WIDTH / 640, 0.7f, 0, "DEAD");
        glColor3f(1, 1, 1);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        GLTexture::disable();
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);//_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    panViewPerspective();

    //leave it for last (rIO)
    drawBorder({ 0.f, 0.f, 0.f, 1.f }, { 0.f, 0.f, 0.f, 0.f }, 80 * WIDTH / 640);

    glDepthMask(GL_TRUE);
}

void skDraw() {
    static int maxscene = (sizeof(timebase) / sizeof(float));
    const float t = skGetTime();

    //  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_LIGHT0);

    glEnable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    //  glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    panViewOrtho();

    if (!skHiddenPart()) {
        static int scene = 0;
        //ARATEMP
        //t+=117.785;
        //
        if ((scene < maxscene) && (t > timebase[scene + 1]))
            scene++;
        if (scene == maxscene) {
            skStopMusic();
            static float closetime = skGetTime();
            float curtime = skGetTime() - closetime;
            if (curtime > 7) {
            	skQuitDemo();
                return;
            }
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            glColor4f(1.f, 1.f, 1.f, 0.6f);
            glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
            glEnable(GL_BLEND);
            //    panViewOrtho();
            scritte->use();
            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
            CoolPrint1(*FontArial,  50, curtime, 0.f, 1.f, 6.f, 7.f, 0.5f * WIDTH,  50 * HEIGHT / 480, 50 * WIDTH / 640, 0.6f, 0.1f, "MekkaSymposium 2002");
            CoolPrint1(*FontArial,  10, curtime, 0.f, 1.f, 6.f, 7.f, 0.5f * WIDTH, 300 * HEIGHT / 480, 20 * WIDTH / 640, 0.8f, 0.f, "this spurious reality hasn't been manufactured by");
            CoolPrint1(*FontArial,  30, curtime, 0.f, 1.f, 6.f, 7.f, 0.5f * WIDTH, 325 * HEIGHT / 480, 30 * WIDTH / 640, 0.8f, 0.1f, "Runciter Associates");
            CoolPrint1(*FontArial,  10, curtime, 0.f, 1.f, 6.f, 7.f, 0.5f * WIDTH, 350 * HEIGHT / 480, 20 * WIDTH / 640, 0.8f, 0.f, "and in any case not in collaboration with");
            CoolPrint1(*FontArial, 100, curtime, 0.f, 1.f, 6.f, 7.f, 0.5f * WIDTH, 385 * HEIGHT / 480, 80 * WIDTH / 640, 0.8f, 0.1f, "SpinningKids");
            glColor3f(1, 1, 1);
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
            GLTexture::disable();
            drawLines(curtime, 0.1f, 100);
            //    return;
        } else {
            Scena(t, scene);
        }
    } else {
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
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
        glColor3f(1, 1, 1);
        CoolPrint1(*FontArial, 20, t, 0.f,  1.f,  6.f,  7.f,  160 * WIDTH / 640,  20 * HEIGHT / 480, 0.0625f * WIDTH, 0.5f, -0.2f, "P.K.Dick Bibliography :");
        CoolPrint1(*FontArial, 20, t, 1.f,  2.f,  7.f,  8.f,  130 * WIDTH / 640,  40 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Solar Lottery (1955)");
        CoolPrint1(*FontArial, 20, t, 2.f,  3.f,  8.f,  9.f,  200 * WIDTH / 640,  60 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "The World Jones Made (1956)");
        CoolPrint1(*FontArial, 20, t, 3.f,  4.f,  9.f,  10.f, 180 * WIDTH / 640,  80 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "The Man Who Japed (1956)");
        CoolPrint1(*FontArial, 20, t, 4.f,  5.f,  10.f, 11.f, 140 * WIDTH / 640, 100 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Eye in the sky (1957)");
        CoolPrint1(*FontArial, 20, t, 5.f,  6.f,  11.f, 12.f, 185 * WIDTH / 640, 120 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "The Cosmic Puppets (1957)");
        CoolPrint1(*FontArial, 20, t, 6.f,  7.f,  12.f, 13.f, 160 * WIDTH / 640, 140 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Time out of Joint (1959)");
        CoolPrint1(*FontArial, 20, t, 7.f,  8.f,  13.f, 14.f, 125 * WIDTH / 640, 160 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Dr.Futurity (1960)");
        CoolPrint1(*FontArial, 20, t, 8.f,  9.f,  14.f, 15.f, 165 * WIDTH / 640, 180 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Vulcan's Hammer (1960)");
        CoolPrint1(*FontArial, 20, t, 9.f,  10.f, 15.f, 16.f, 195 * WIDTH / 640, 200 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Man in the high castle (1963)");
        CoolPrint1(*FontArial, 20, t, 10.f, 11.f, 16.f, 17.f, 200 * WIDTH / 640, 220 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Game Players of Titan (1963)");
        CoolPrint1(*FontArial, 20, t, 11.f, 12.f, 17.f, 18.f, 185 * WIDTH / 640, 240 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Martian Time Sleep (1964)");
        CoolPrint1(*FontArial, 20, t, 12.f, 13.f, 18.f, 19.f, 140 * WIDTH / 640, 260 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "The Simulacra (1964)");
        CoolPrint1(*FontArial, 20, t, 13.f, 14.f, 19.f, 20.f, 220 * WIDTH / 640, 280 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Clans of the Alphane Moon (1964)");
        CoolPrint1(*FontArial, 20, t, 14.f, 15.f, 20.f, 21.f, 200 * WIDTH / 640, 300 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "The Penultimate Truth (1964)");
        CoolPrint1(*FontArial, 20, t, 15.f, 16.f, 21.f, 22.f, 290 * WIDTH / 640, 320 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "The tree stigmata of Palmer Eldritch (1965)");
        CoolPrint1(*FontArial, 20, t, 16.f, 17.f, 22.f, 23.f, 145 * WIDTH / 640, 340 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Dr.Bloodmoney (1965)");
        CoolPrint1(*FontArial, 20, t, 17.f, 18.f, 23.f, 24.f, 180 * WIDTH / 640, 360 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "The crack in space (1966)");
        CoolPrint1(*FontArial, 20, t, 18.f, 19.f, 24.f, 25.f, 190 * WIDTH / 640, 380 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "No wait for last year (1966)");
        CoolPrint1(*FontArial, 20, t, 19.f, 20.f, 25.f, 26.f, 200 * WIDTH / 640, 400 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "The unteleported man (1966)");
        CoolPrint1(*FontArial, 20, t, 20.f, 21.f, 26.f, 27.f, 195 * WIDTH / 640, 420 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Counter clock world (1967)");
        CoolPrint1(*FontArial, 20, t, 21.f, 22.f, 27.f, 28.f, 135 * WIDTH / 640,  20 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "The zap gun (1967)");
        CoolPrint1(*FontArial, 20, t, 22.f, 23.f, 28.f, 29.f, 210 * WIDTH / 640,  40 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "The Ganymede takeover (1967)");
        CoolPrint1(*FontArial, 20, t, 23.f, 24.f, 29.f, 30.f, 310 * WIDTH / 640,  60 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Do androids dream of electric sheeps? (1968)");
        CoolPrint1(*FontArial, 20, t, 24.f, 25.f, 30.f, 31.f,  85 * WIDTH / 640,  80 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Ubik (1969)");
        CoolPrint1(*FontArial, 20, t, 25.f, 26.f, 31.f, 32.f, 190 * WIDTH / 640, 100 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Galactic Pot-Healer (1969)");
        CoolPrint1(*FontArial, 20, t, 26.f, 27.f, 32.f, 33.f, 145 * WIDTH / 640, 120 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Maze of death (1970)");
        CoolPrint1(*FontArial, 20, t, 27.f, 28.f, 33.f, 34.f, 220 * WIDTH / 640, 140 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Our friends from Frolix 8 (1970)");
        CoolPrint1(*FontArial, 20, t, 28.f, 29.f, 34.f, 35.f, 165 * WIDTH / 640, 160 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "We can build you (1972)");
        CoolPrint1(*FontArial, 20, t, 29.f, 30.f, 35.f, 36.f, 275 * WIDTH / 640, 180 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Flow my tears the policeman said (1974)");
        CoolPrint1(*FontArial, 20, t, 30.f, 31.f, 36.f, 37.f, 240 * WIDTH / 640, 200 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Confessions of a crap artist (1975)");
        CoolPrint1(*FontArial, 20, t, 31.f, 32.f, 37.f, 38.f, 125 * WIDTH / 640, 220 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Deus irae (1976)");
        CoolPrint1(*FontArial, 20, t, 32.f, 33.f, 38.f, 39.f, 165 * WIDTH / 640, 240 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "A scanner darkly (1977)");
        CoolPrint1(*FontArial, 20, t, 33.f, 34.f, 39.f, 40.f,  85 * WIDTH / 640, 260 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Valis (1981)");
        CoolPrint1(*FontArial, 20, t, 34.f, 35.f, 40.f, 41.f, 185 * WIDTH / 640, 280 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "The divine Invasion (1981)");
        CoolPrint1(*FontArial, 20, t, 35.f, 36.f, 41.f, 42.f, 305 * WIDTH / 640, 300 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "The trasmigrating of Timothy Archer (1982)");
        CoolPrint1(*FontArial, 20, t, 36.f, 37.f, 42.f, 43.f, 285 * WIDTH / 640, 320 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "The man whose ttet where all alike (1984)");
        CoolPrint1(*FontArial, 20, t, 37.f, 38.f, 43.f, 44.f, 190 * WIDTH / 640, 340 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Radio free Albemuth (1985)");
        CoolPrint1(*FontArial, 20, t, 38.f, 39.f, 44.f, 45.f, 265 * WIDTH / 640, 360 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Puttering about in a small land (1985)");
        CoolPrint1(*FontArial, 20, t, 39.f, 40.f, 45.f, 46.f, 220 * WIDTH / 640, 380 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "In Milton Lumky Territoru (1985)");
        CoolPrint1(*FontArial, 20, t, 40.f, 41.f, 46.f, 47.f, 230 * WIDTH / 640, 400 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Humpy dumpty in Hoackland (1986)");
        CoolPrint1(*FontArial, 20, t, 41.f, 42.f, 47.f, 48.f, 185 * WIDTH / 640, 420 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Mary and the Giant (1987)");
        CoolPrint1(*FontArial, 20, t, 42.f, 43.f, 48.f, 49.f, 170 * WIDTH / 640, 440 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "The broken bubble (1988)");
        CoolPrint1(*FontArial, 20, t, 43.f, 44.f, 49.f, 50.f, 220 * WIDTH / 640, 460 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "Gather yourself together (1994)");
        CoolPrint1(*FontArial, 20, t, 50.f, 51.f, 5000.f, 5000.f, 300 * WIDTH / 640, 230 * HEIGHT / 480, 0.0625f * WIDTH, 0.4f, -0.2f, "are you dreaming of electric sheeps ?");
    }
}

void skInitDemoStuff()
{

    // pan stuff
    FontArial = skLoadFont("Arial");

    glClearColor(0.f, 0.f, 0.f, 0.5f);						// Black Background
    glClearDepth(1.0);										// Depth Buffer Setup
    glDepthFunc(GL_LEQUAL);									// The Type Of Depth Testing (Less Or Equal)
    glEnable(GL_DEPTH_TEST);									// Enable Depth Testing
    glShadeModel(GL_SMOOTH);									// Select Smooth Shading
//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Set Perspective Calculations To Most Accurate
    glDisable(GL_CULL_FACE);
    glEnable(GL_COLOR_MATERIAL);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

    //glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    //glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

    glClear(GL_COLOR_BUFFER_BIT);

    //inizializzo i valori di defautl del particle system
    //e le relative textures

    //inizializzo lo sfondo
    m_glqMyQuadratic = gluNewQuadric();
    gluQuadricNormals(m_glqMyQuadratic, GLU_SMOOTH);
    gluQuadricTexture(m_glqMyQuadratic, GL_TRUE);

    int render_to_tex_log_size = 12;
    while (std::min(WIDTH, HEIGHT) < (1 << render_to_tex_log_size)) {
        --render_to_tex_log_size;
    }

    env = perlin(3, 4, 0.6f, 0.4f, 1, true);
    frame = perlin(render_to_tex_log_size, 1, 0.3f, 0.7f, 2, true); // ma cristodundio! I parametri no a caso! - modificato in 8
    frame2 = perlin(render_to_tex_log_size, 1, 0.3f, 0.7f, 2, true); // ma cristodundio! I parametri no a caso! - modificato in 8
    //int a = frame2->getSize();

    toro = perlin(4, 5, 0.4f, 0.5f, 1.5, true);
    cubo = perlin(5, 16, 0.2f, 0.3f, 2.2f, true);
    texture1 = perlin(8, 3000, 0.3f, 0.5, 1.8f, false);
    texture2 = perlin(8, 300, 0.1f, 0.97f, 3, false);
    tex = perlin(3, 1, 0.4f, 0.5f, 2, true);
    tex2 = perlin(8, 132, 0.3f, 0.7f, 2, true);
    scritte = perlin(7, 100, 0.6f, 0.5f, 1.3f, true);// usare per le scritte

  //LOD, radius
    initSphereObject(40, 2);

    logo = CreatePKLogoTexture();
}

void skUnloadDemoStuff()
{
  gluDeleteQuadric(m_glqMyQuadratic);

  glDeleteTextures(1,&logo);
  unloadSphereEffect();

  //delete [] env;
}
