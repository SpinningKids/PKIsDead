
#ifndef ASMMATH2_H_
#define ASMMATH2_H_

#include <memory>

struct Vector3 {
    float x{}, y{}, z{};

    float& operator[](int ind) {
        return (&x)[ind];
    }

    const float& operator[](int ind) const {
        return (&x)[ind];
    }

    Vector3 operator - () const {
        return { -x, -y, -z };
    }

    Vector3 operator + (const Vector3& b) const {
        return { x + b.x, y + b.y, z + b.z };
    }

    Vector3 operator - (const Vector3& b) const {
        return { x - b.x, y - b.y, z - b.z };
    }

    Vector3 operator * (float b) const {
        return { x * b, y * b, z * b };
    }

    Vector3 operator / (float b) const {
        return (*this) * (1.f / b);
    }

    //dot
    float operator * (const Vector3& b) const {
        return x * b.x + y * b.y + z * b.z;
    }

    //cross
    Vector3 operator ^ (const Vector3& b) const {
        return { y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x };
    }

    //normalized dot
    float operator | (Vector3& b) const {
        return *this * b / sqrtf((*this) * (*this) * (b * b)); // il caso nullo e' molto raro, dovresti gestirlo anche in / a questo punto!
    }

    //euqual to
    bool operator == (Vector3 const& b) const {
        return(x == b.x && y == b.y && z == b.z);
    }

    //differs from
    bool operator != (Vector3 const& b) const {
        return(x != b.x || y != b.y || z != b.z);
    }

    Vector3& operator += (const Vector3& a) {
        x += a.x;
        y += a.y;
        z += a.z;
        return *this;
    }

    Vector3& operator -= (const Vector3& a) {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        return *this;
    }

    Vector3& operator *= (float a) {
        x *= a;
        y *= a;
        z *= a;
        return *this;
    }

    Vector3& operator /= (float a) {
        x /= a;
        y /= a;
        z /= a;
        return *this;
    }

    Vector3& operator ^= (const Vector3& b) {
        float x1 = y * b.z - z * b.y;
        float y1 = z * b.x - x * b.z;
        z = x * b.y - y * b.x;
        x = x1;
        y = y1;
        return *this;
    }

    // Secondo me non ha senso, comunque te lo lascio...
    Vector3 Abs() const {
        Vector3 res(*this);

        if (res.x < 0.f) res.x = -res.x;
        if (res.y < 0.f) res.y = -res.y;
        if (res.z < 0.f) res.z = -res.z;

        return res;
    }

    void Null() {
        x = y = z = 0.f;
    }

    void Invert() { // ma servono ?!?
        x = 1.f / x;
        y = 1.f / y;
        z = 1.f / z;
    }

    Vector3 Inverted() const {
        return { 1.f / x, 1.f / y, 1.f / z };
    }

    void Scale(const Vector3& sc) {
        x *= sc.x;
        y *= sc.y;
        z *= sc.z;
    }

    void Scale(float sc) {
        x *= sc;
        y *= sc;
        z *= sc;
    }

    Vector3 Scaled(const Vector3& sc) const {
        return { x * sc.x, y * sc.y, z * sc.z };
    }

    Vector3 Scaled(float sc) const {
        return { x * sc, y * sc, z * sc };
    }

    float LengthSquare() const {
        return (x * x + y * y + z * z);
    }

    float Length() const {
        return sqrtf(LengthSquare());
    }

    void Normalize() {
        *this /= Length();
    }

    Vector3 Normalized() const {
        return (*this) / Length();
    }

    //this returns a vector r that is the result of the current vector
    //bouncing on vector n
    Vector3 Reflected(const Vector3& n) const {
        return (*this) - (n * (2.f * (n * (*this))));
    }

    void Reflect(const Vector3& n) {
        *this -= n * (2.f * (n * (*this)));
    }
};

inline Vector3 operator*(float a, const Vector3& b) {
    return { a * b.x, a * b.y, a * b.z };
}

inline Vector3 CalcNormal(const Vector3& p, const Vector3& p1, const Vector3& p2) {
    return ((p1 - p2) ^ (p2 - p)).Normalized();
}

#endif // ASMMATH2_H_
