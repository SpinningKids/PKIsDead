// SphereEffect.cpp


////////////////////////////////////////////////////////
//		includes

#include "SphereEffect.h"

#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#endif /* WIN32 */
#include <gl/GL.h>
#include <cassert>
#include "asmmath2.h"

////////////////////////////////////////////////////////
//		globals

static int			g_subd;
static int			g_numVertex = 0;
static Vector3* g_pVertex = nullptr;

static Vector3		g_pos;

////////////////////////////////////////////////////////
//

// subd = 40, radius = 2
int initSphereObject(int subd, float radius) {
    int i = 0, j = 0, offset = 0;

    assert(subd);

    g_subd = subd;
    g_numVertex = (g_subd / 2) * (g_subd * 2);
    g_pVertex = new Vector3[g_numVertex];

    if (!g_pVertex)
        return -1;

    float inc_phi = TWOPI / g_subd;
    float inc_theta = PI / g_subd;

    // modify the sphere shape
    for (int i = 0; i < g_subd / 2; ++i) {
        float phi = inc_phi * i;
        for (int j = 0; j < g_subd * 2; ++j) {
            float theta = inc_theta * j;
            float rad = radius * (radius + 0.5f * sinf((i + j) * inc_phi) * cosf(3 * (i + j) * inc_phi));

            float sinfphi = sinf(phi);
            g_pVertex[offset].x = rad * sinfphi * cosf(theta);
            g_pVertex[offset].y = rad * sinfphi * sinf(theta);
            g_pVertex[offset].z = rad * cosf(phi);

            ++offset;
        }
    }

    g_pos.x = 0;
    g_pos.y = 0;
    g_pos.z = -15;

    return 0;
}



void dSphereEffect(const rgb_a& pcolor, const rgb_a& vcolor, bool base) {
    assert(g_numVertex && g_subd && g_pVertex);

    glPushMatrix();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBegin(GL_LINE_STRIP);

    glColor4f(pcolor.r, pcolor.g, pcolor.b, pcolor.a);
    for (int offset = 0; offset < g_numVertex; ++offset) {
        glVertex3f(g_pVertex[offset].x,
            g_pVertex[offset].y,
            g_pVertex[offset].z);
    }

    glEnd();

    if (base) {

        int offset = 0;

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        int vertexPerSpan = g_numVertex / g_subd;

        for (int y = 0; y < g_subd - 1; y++) {
            glBegin(GL_TRIANGLE_STRIP);

            glColor4f(vcolor.r, vcolor.g, vcolor.b, vcolor.a);
            for (int x = 0; x < vertexPerSpan; x++) {
                glVertex3f(g_pVertex[offset].x,
                    g_pVertex[offset].y,
                    g_pVertex[offset].z);

                glVertex3f(g_pVertex[offset].x,
                    g_pVertex[offset + vertexPerSpan].y,
                    g_pVertex[offset].z);

                ++offset;
            }

            glEnd();
        }
    }
    glPopMatrix();
}

void unloadSphereEffect() {
    delete[] g_pVertex;
}
