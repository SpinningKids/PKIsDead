#ifndef ASMMATH4_H_
#define ASMMATH4_H_

#include <cstdlib>

constexpr float TWOPI = 6.2831853071795f;
constexpr float HALFPI = 1.5707963267948f;
constexpr float PI = 3.1415926535897932384626433832795f;
constexpr float PIOVER180 = 0.0174532925199432957692369076848861f;
constexpr float PIUNDER180 = 57.2957795130823208767981548141052f;
constexpr float TWOPI10 = 62.831853071795f;
constexpr float DTOR = 0.0174532925f;
constexpr float RTOD = 57.2957795f;

//#define USE_ASM_VER

struct uv_coord {
    float u, v;
};

struct rgb_a {
    float r, g, b, a;

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
    return { -a.r, -a.g, -a.b, -a.a };
}

inline rgb_a operator+(const rgb_a& a, const rgb_a& b) {
    return { a.r + b.r, a.g + b.g, a.b + b.b, a.a + b.a };
}

inline rgb_a operator-(const rgb_a& a, const rgb_a& b) {
    return { a.r - b.r, a.g - b.g, a.b - b.b, a.a - b.a };
}

inline rgb_a operator*(const rgb_a& a, const float b) {
    return { a.r * b, a.g * b, a.b * b, a.a * b };
}

inline rgb_a operator*(float a, const rgb_a& b) {
    return { a * b.r, a * b.g, a * b.b, a * b.a };
}

inline rgb_a operator/(const rgb_a& a, const float b) {
    float ib = 1.f / b;
    return { a.r * ib, a.g * ib, a.b * ib, a.a * ib };
}

inline float Random(float min, float max) {
    return ((rand() % ((int)(max * 10) - (int)(min + 1 * 10))) + min * 10) / 10.f;
};

//returns the colour between start and end at a "where" position (range 0,1)
inline rgb_a GetFade(rgb_a start, rgb_a end, float where) {
    return start + (end - start) * where;
}

#endif // ASMMATH4_H_
