#ifndef ASM_MATH3_H_
#define ASM_MATH3_H_

#include <stdlib.h>

#define TWOPI           6.2831853071795f
#define HALFPI			1.5707963267948f
#define PI				3.1415926535897932384626433832795f
#define PIOVER180		0.0174532925199432957692369076848861f
#define PIUNDER180		57.2957795130823208767981548141052f
#define TWOPI10			62.831853071795f
#define DTOR            0.0174532925f
#define RTOD            57.2957795f

//#define USE_ASM_VER

struct uv_coord {
    float u, v;

    uv_coord() = default;

    uv_coord(uv_coord const& uv) = default;
    uv_coord& operator = (uv_coord const& uv) = default;
    uv_coord(uv_coord&& uv) = default;
    uv_coord& operator = (uv_coord&& uv) = default;

    uv_coord(const float pu, const float pv) : u(pu), v(pv) {}

};

struct rgb_a
{
    float r, g, b, a;

    rgb_a() {}

    rgb_a(rgb_a const& c) = default;
    rgb_a& operator = (rgb_a const& v) = default;
    rgb_a(rgb_a&& c) = default;
    rgb_a& operator = (rgb_a&& v) = default;

    rgb_a(const float cr, const float cg, const float cb, const float ca) : r(cr), g(cg), b(cb), a(ca) {}

    float& operator[](const int ind) {
        if (ind == 0) return r;
        if (ind == 1) return g;
        if (ind == 2) return b;
        return a;
    }

    const float& operator[](const int ind) const {
        if (ind == 0) return r;
        if (ind == 1) return g;
        if (ind == 2) return b;
        return a;
    }

    void Set(const float ar, const float ag, const float ab, const float aa) {
        this->r = ar;
        this->g = ag;
        this->b = ab;
        this->a = aa;
    }

    rgb_a& operator+=(const rgb_a& c) {
        r += c.r;
        g += c.g;
        b += c.b;
        a += c.a;
        return *this;
    }

    rgb_a& operator+=(const float& c) {
        r += c;
        g += c;
        b += c;
        a += c;
        return *this;
    }

    rgb_a& operator-=(const rgb_a& c) {
        r -= c.r;
        g -= c.g;
        b -= c.b;
        a -= c.a;
        return *this;
    }

    rgb_a& operator-=(const float& c) {
        r -= c;
        g -= c;
        b -= c;
        a -= c;
        return *this;
    }

    rgb_a& operator*=(const float c) {
        r *= c;
        g *= c;
        b *= c;
        a *= c;
        return *this;
    }

    rgb_a& operator/=(const float c) {
        r /= c;
        g /= c;
        b /= c;
        a /= c;
        return *this;
    }
};

inline rgb_a operator-(const rgb_a& a) {
    return rgb_a(-a.r, -a.g, -a.b, -a.a);
}

inline rgb_a operator+(const rgb_a& a, const rgb_a& b) {
    return rgb_a(a.r + b.r, a.g + b.g, a.b + b.b, a.a + b.a);
}

inline rgb_a operator-(const rgb_a& a, const rgb_a& b) {
    return rgb_a(a.r - b.r, a.g - b.g, a.b - b.b, a.a - b.a);
}

inline rgb_a operator*(const rgb_a& a, const float b) {
    return rgb_a(a.r * b, a.g * b, a.b * b, a.a * b);
}

inline rgb_a operator*(float a, const rgb_a& b) {
    return rgb_a(a * b.r, a * b.g, a * b.b, a * b.a);
}

inline rgb_a operator/(const rgb_a& a, const float b) {
    float ib = 1.f / b;
    return rgb_a(a.r * ib, a.g * ib, a.b * ib, a.a * ib);
}

inline float Random(float min, float max) {
    return ((rand() % ((int)(max * 10) - (int)(min + 1 * 10))) + min * 10) / 10.0f;
};

//returns the colour between start and end at a "where" position (range 0,1)
inline rgb_a GetFade(rgb_a start, rgb_a end, float where) {
    rgb_a tmp;

    tmp.r = start.r + ((end.r - start.r) * where);
    tmp.g = start.g + ((end.g - start.g) * where);
    tmp.b = start.b + ((end.b - start.b) * where);
    tmp.a = start.a + ((end.a - start.a) * where);

    return tmp;
    //return ( start + ((end - start) * where) );
}

#include "asmmath2.h"

#endif // ASM_MATH3_H_
