
#ifndef ASMMATH2_H_
#define ASMMATH2_H_

#include <memory>

struct Vector3 {
    float x{}, y{}, z{};

    constexpr Vector3 operator - () const {
        return { -x, -y, -z };
    }

    constexpr Vector3 operator + (const Vector3& b) const {
        return { x + b.x, y + b.y, z + b.z };
    }

    constexpr Vector3 operator - (const Vector3& b) const {
        return { x - b.x, y - b.y, z - b.z };
    }

    constexpr Vector3 operator * (float b) const {
        return { x * b, y * b, z * b };
    }

    constexpr Vector3 operator / (float b) const {
        return (*this) * (1.f / b);
    }

    //dot
    constexpr float operator * (const Vector3& b) const {
        return x * b.x + y * b.y + z * b.z;
    }

    //cross
    constexpr Vector3 operator ^ (const Vector3& b) const {
        return { y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x };
    }

    //normalized dot
    float operator | (Vector3& b) const {
        return *this * b / sqrtf((*this) * (*this) * (b * b)); // il caso nullo e' molto raro, dovresti gestirlo anche in / a questo punto!
    }

    //euqual to
    constexpr bool operator == (Vector3 const& b) const {
        return(x == b.x && y == b.y && z == b.z);
    }

    //differs from
    constexpr bool operator != (Vector3 const& b) const {
        return(x != b.x || y != b.y || z != b.z);
    }

    constexpr Vector3& operator += (const Vector3& a) {
        x += a.x;
        y += a.y;
        z += a.z;
        return *this;
    }

    constexpr Vector3& operator -= (const Vector3& a) {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        return *this;
    }

    constexpr Vector3& operator *= (float a) {
        x *= a;
        y *= a;
        z *= a;
        return *this;
    }

    constexpr Vector3& operator /= (float a) {
        x /= a;
        y /= a;
        z /= a;
        return *this;
    }

    constexpr Vector3& operator ^= (const Vector3& b) {
        float x1 = y * b.z - z * b.y;
        float y1 = z * b.x - x * b.z;
        z = x * b.y - y * b.x;
        x = x1;
        y = y1;
        return *this;
    }

    // Secondo me non ha senso, comunque te lo lascio...
    constexpr Vector3 Abs() const {
        Vector3 res(*this);

        if (res.x < 0.f) res.x = -res.x;
        if (res.y < 0.f) res.y = -res.y;
        if (res.z < 0.f) res.z = -res.z;

        return res;
    }

    constexpr void Null() {
        x = y = z = 0.f;
    }

    constexpr void Invert() { // ma servono ?!?
        x = 1.f / x;
        y = 1.f / y;
        z = 1.f / z;
    }

    constexpr Vector3 Inverted() const {
        return { 1.f / x, 1.f / y, 1.f / z };
    }

    constexpr void Scale(const Vector3& sc) {
        x *= sc.x;
        y *= sc.y;
        z *= sc.z;
    }

    constexpr void Scale(float sc) {
        x *= sc;
        y *= sc;
        z *= sc;
    }

    constexpr Vector3 Scaled(const Vector3& sc) const {
        return { x * sc.x, y * sc.y, z * sc.z };
    }

    constexpr Vector3 Scaled(float sc) const {
        return { x * sc, y * sc, z * sc };
    }

    constexpr float LengthSquare() const {
        return (x * x + y * y + z * z);
    }

    float Length() const {
        return sqrtf(LengthSquare());
    }

    constexpr void Normalize() {
        *this /= Length();
    }

    constexpr Vector3 Normalized() const {
        return (*this) / Length();
    }

    //this returns a vector r that is the result of the current vector
    //bouncing on vector n
    constexpr Vector3 Reflected(const Vector3& n) const {
        return (*this) - (n * (2.f * (n * (*this))));
    }

    constexpr void Reflect(const Vector3& n) {
        *this -= n * (2.f * (n * (*this)));
    }
};

constexpr Vector3 operator*(float a, const Vector3& b) {
    return { a * b.x, a * b.y, a * b.z };
}

inline Vector3 CalcNormal(const Vector3& p, const Vector3& p1, const Vector3& p2) {
    return ((p1 - p2) ^ (p2 - p)).Normalized();
}

#endif // ASMMATH2_H_
