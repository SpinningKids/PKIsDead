// SphereEffect.cpp


////////////////////////////////////////////////////////
//		includes

#include <assert.h>
#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#endif /* WIN32 */
#include <gl/GL.h>
#include "SphereEffect.h"
#include "AsmMath4.h"

////////////////////////////////////////////////////////
//		globals

static int			g_subd;
static int			g_numVertex = 0;
static Vector3* g_pVertex = nullptr;

static float	g_toRad = (2.0f * 3.141592653589f / 360.0f);

static Vector3		g_pos;

////////////////////////////////////////////////////////
//

// subd = 40, radius = 2
int initSphereObject(int subd, float radius) {
    int i = 0, j = 0, offset = 0;
    float angle = 360.0f / subd;

    assert(subd);

    g_subd = subd;
    g_numVertex = g_subd * g_subd;
    //rIO
  //g_pVertex = (Vector3 *) malloc( sizeof(Vertex) * g_numVertex );	
    g_pVertex = (Vector3*) new Vector3[g_numVertex];

    if (!g_pVertex)
        return -1;

    float inc_phi = 360.0f / g_subd;
    float inc_theta = 180.0f / g_subd;

    // modify the sphere shape
    for (float phi = 0; phi < 180; phi += inc_phi, i++) {
        for (float theta = 0; theta < 360; theta += inc_theta, j++) {
            float rad = (radius + 0.5f * sinf(g_toRad * (i * angle + j * angle)) * cosf(g_toRad * (3 * (i + j) * angle)));

            g_pVertex[offset].x = rad * radius * sinf(phi * g_toRad) * cosf(theta * g_toRad);
            g_pVertex[offset].y = rad * radius * sinf(phi * g_toRad) * sinf(theta * g_toRad);
            g_pVertex[offset].z = rad * radius * cosf(phi * g_toRad);

            ++offset;
        }
    }

    g_pos.x = 0;
    g_pos.y = 0;
    g_pos.z = -15;

    return 0;
}



void dSphereEffect(rgb_a pcolor, rgb_a vcolor, bool base) {
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
    glDisable(GL_FOG);
}
