
/************************************
  REVISION LOG ENTRY
  Revision By: Pan
  Revised on 2/7/2002 11:58:38 PM
  Comments: Creation
 ************************************/

#include "CoolPrint.h"

#include "AsmMath4.h"

#include <stdio.h>
#include <algorithm>

void CoolPrint1(GLFont const& font, int n, float t, float st, float on, float fd, float en, float x, float y, float gs, float ratio, float tracking, const char* fmt, ...) {
    if (t < st - 0.5f) return;
    if (t > en) return;

    t -= st;
    on -= st;
    fd -= st;
    en -= st;

    char text[256];
    va_list		ap;

    if (fmt == nullptr)
        return;
    va_start(ap, fmt);
#ifdef WIN32
    vsprintf_s(text, fmt, ap);
#else
    vsnprintf(text, fmt, ap);
#endif
    va_end(ap);

    unsigned int l = strlen(text);

    float don = 0.5f * on / l;
    float doff = 0.5f * (en - fd) / l;

    float iadj = 100.f / n;
    float cadj = 17.25f / n;

    float width = 0;
    float height = 0;
    for (unsigned int i = 0; i < l; i++) {
        width += font.getWidth(text[i], tracking);
        height = std::max(height, font.getHeight(text[i]));
    }

    x -= gs * width / 2;
    y += gs * height / 2;

    for (int j1 = 0; j1 < n; j1++) {
        float i = j1 * iadj;

        glPushMatrix();
        glLoadIdentity();
        glTranslatef(x, y, 0);
        glScalef(gs, -gs, 1);
        float t1 = t + don;
        float t2 = t - fd + doff;
        for (unsigned int j = 0; j < l; j++) {
            float tim;
            t1 -= don;
            t2 -= doff;
            if (t1 < 0.5f * on) {
                tim = (PI * t1 / on);
            } else {
                if (t2 > 0) {//(en-fd)*0.5) {
                    if (t2 < 0.5f * (en - fd)) {
                        tim = (PI * 0.5f + PI * t2 / (en - fd));
                    } else {
                        tim = PI;
                    }
                } else {
                    tim = 0.5f * (PI);
                }
            }
            tim += i / 200.f;
            float q = (1 - cosf(2 * tim)) / 2;
            if ((tim < 0) || (tim > PI) || (q < 0.01f)) {
                font.skipChar(text[j], tracking);
            } else {
                const float desade = 0.25;
                float r = cadj * (i / 100.f) * q;
                float g = cadj * (1 - i / 100.f) * q;
                float b = cadj * (fabsf(i / 50 - 1)) * q;
                glColor3f(r * desade + (b + g) * 0.5f * (1.f - desade), g * desade + (r + b) * 0.5f * (1.f - desade), b * desade + (r + g) * 0.5f * (1.f - desade));
                float rot = -(sinf(tim * 2.f) + tim * 2.f) * 2.f * PIUNDER180 + 20.f * rand() / static_cast<float>(RAND_MAX) - 10.f;
                font.printChar(text[j], 0.8f * (q + 0.2f) * (1.f + rand() / static_cast<float>(RAND_MAX)), 0.8f * ratio * (q + 0.2f) * (1.f + rand() / static_cast<float>(RAND_MAX)), rot, tracking);
            }
        }
        glPopMatrix();										// Pops The Display List Bits
    }
}
