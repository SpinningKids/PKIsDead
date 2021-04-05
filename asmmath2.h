
#ifndef _ASMMATH2_
#define _ASMMATH2_

#include <memory>
#include <utility>

struct Vector3;
struct Matrix4;

struct Vector3 {
    float x, y, z;

    Vector3() = default;

    Vector3(Vector3 const& v) = default;
    Vector3& operator = (Vector3 const& v) = default;
    Vector3(Vector3&& v) = default;
    Vector3& operator = (Vector3 && v) = default;

    Vector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}

    float& operator[](int ind) {
        return (&x)[ind];
    }

    const float& operator[](int ind) const {
        return (&x)[ind];
    }

    void Set(float ax, float ay, float az) {
        this->x = ax;
        this->y = ay;
        this->z = az;
    }

    Vector3 operator - () const {
        return Vector3(-x, -y, -z);
    }

    Vector3 operator + (const Vector3& b) const {
        return Vector3(x + b.x, y + b.y, z + b.z);
    }

    Vector3 operator - (const Vector3& b) const {
        return Vector3(x - b.x, y - b.y, z - b.z);
    }

    Vector3 operator * (float b) const {
        return Vector3(x * b, y * b, z * b);
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
        return Vector3(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
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

        if (res.x < 0.0f) res.x = -res.x;
        if (res.y < 0.0f) res.y = -res.y;
        if (res.z < 0.0f) res.z = -res.z;

        return res;
    }

    void Null() {
        x = y = z = 0.0f;
    }

    /*
      void Negate() { // cassata - usare  v = -v;
        x = -x;
        y = -y;
        z = -z;
      }

      Vector3 Negated() { // Non serve!
        return Vector3(-x, -y ,-z);
      }
    */

    void Invert() { // ma servono ?!?
        x = 1.0f / x;
        y = 1.0f / y;
        z = 1.0f / z;
    }

    Vector3 Inverted() const {
        return Vector3(1.0f / x, 1.0f / y, 1.0f / z);
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
        return Vector3(x * sc.x, y * sc.y, z * sc.z);
    }

    Vector3 Scaled(float sc) const {
        return Vector3(x * sc, y * sc, z * sc);
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
        return (*this) - (n * (2.0f * (n * (*this))));
    }

    void Reflect(const Vector3& n) {
        *this -= n * (2.0f * (n * (*this)));
    }

    /*
      void Transform(const float* mat) {
            float x1 = x*mat[0] + y*mat[4]+ z*mat[8] + mat[12];
            float y1 = x*mat[1] + y*mat[5]+ z*mat[9] + mat[13];
            z = x*mat[2] + y*mat[6]+ z*mat[10] + mat[14];
        x = x1;
        y = y1;
      }
    */

    Vector3 TransformedNormal(const Matrix4& mat) const;
    void TransformNormal(const Matrix4& mat);
    Vector3& operator*=(const Matrix4& m);
    Vector3& operator/=(const Matrix4& m);
};

inline Vector3 operator*(float a, const Vector3& b) {
    return Vector3(a * b.x, a * b.y, a * b.z);
}

inline Matrix4 operator*(const Matrix4& a, const Matrix4& b);

struct Matrix4
{
public:
    float xx, xy, xz, dx;
    float yx, yy, yz, dy;
    float zx, zy, zz, dz;
    float wx, wy, wz, dw;

    Matrix4() {
        this->Identity(); // questa per ora te la concedo...
    }

    explicit Matrix4(const Vector3& v) {
        SetTranslation(v);
    }

    float& operator[](int ind) {
        return (&xx)[ind];
    }

    float operator[](int ind) const {
        return (&xx)[ind];
    }

    float* Get() {// okkio ai side effects!
        return (float*)&xx;
    }

    //rIO : holy shit, got mad for a typo in this one !!
    void Set(float* m) {
        memcpy(&xx, m, 16 * sizeof(float));
        //    xx = m[0]; xy = m[1]; xz = m[2]; dx = m[3];
        //    yx = m[4]; yy = m[5]; yz = m[6]; dy = m[7];
        //    zx = m[8]; zy = m[9]; zz = m[10]; dz = m[11];
        //    wx = m[12]; wy = m[13]; wz = m[14]; dw = m[15];
    }

    void Identity() {
        xy = xz = dx = 0.f;
        yx = yz = dy = 0.f;
        zx = zy = dz = 0.f;
        wx = wy = wz = 0.0f;
        xx = yy = zz = dw = 1.0f;
    }

    void Transpose() {
        std::swap(xy, yx);
        std::swap(xz, zx);
        std::swap(dx, wx);
        std::swap(yz, zx);
        std::swap(dy, wy);
        std::swap(dz, wz);
    }

    //remember this needs to transpose the matrix
    Vector3 GetUpBillboardVector() const {
        return Vector3(yx, yy, yz);
    }

    //remember this needs to transpose the matrix
    Vector3 GetRightBillboardVector() const {
        return Vector3(xx, xy, xz);
    }

    void Inverse() {
        Matrix4 r;
        float d = 1.0f / (
            xx * (yy * zz - yz * zy) -
            xy * (yx * zz - yz * zx) +
            xz * (yx * zy - yy * zx));


        r.xx = d * (yy * zz - yz * zy);
        r.xy = -d * (xy * zz - xz * zy);
        r.xz = d * (xy * yz - xz * yy);

        r.yx = -d * (yx * zz - yz * zx);
        r.yy = d * (xx * zz - xz * zx);
        r.yz = -d * (xx * yz - xz * yx);

        r.zx = d * (yx * zy - yy * zx);
        r.zy = -d * (xx * zy - xy * zx);
        r.zz = d * (xx * yy - xy * yx);

        r.wx = -(wx * r.xx + wy * r.yx + wz * r.zx);
        r.wy = -(wx * r.xy + wy * r.yy + wz * r.zy);
        r.wz = -(wx * r.xz + wy * r.yz + wz * r.zz);

        *this = r;

    }

    void Translate(const Vector3& v) {
        wx += v.x;
        wy += v.y;
        wz += v.z;
    }

    void SetTranslation(const Vector3& v) {
        wx = v.x;
        wy = v.y;
        wz = v.z;
    }

    Vector3 GetTranslation() const {
        return Vector3(wx, wy, wz);
    }

    void GetInverseTranslateVec(Vector3* pVect) const {
        pVect->x = pVect->x - wx;
        pVect->y = pVect->y - wy;
        pVect->z = pVect->z - wz;
    }

    void GetInverseRotateVec(Vector3* pVect) const {
        Vector3 vec;

        vec.x = pVect->x * xx + pVect->y * xy + pVect->z * xz;
        vec.y = pVect->x * yx + pVect->y * yy + pVect->z * yz;
        vec.z = pVect->x * zx + pVect->y * zy + pVect->z * zz;

        pVect->x = vec.x;
        pVect->y = vec.y;
        pVect->z = vec.z;

    }

    void Scale(float s) {
        Identity();
        xx = s;
        yy = s;
        zz = s;
    }

    void Scale(const Vector3& v) {
        Identity();
        xx = v.x;
        yy = v.y;
        zz = v.z;
    }

    void SetRotation(const Vector3& vd) {
        Vector3 v = vd;

        //gotta convert to radians 
        v *= (float)DTOR;

        float sx = sinf(v.x);
        float sy = sinf(v.y);
        float sz = sinf(v.z);
        float cx = cosf(v.x);
        float cy = cosf(v.y);
        float cz = cosf(v.z);

        xx = cy * cz;
        xy = cy * sz;
        xz = -sy;
        yx = sx * sy * cz - cx * sz;
        yy = sx * sy * sz + cx * cz;
        yz = sx * cy;
        zx = cx * sy * cz + sx * sz;
        zy = cx * sy * sz - sx * cz;
        zz = cx * cy;
    }

    void SetRotationRadians(const Vector3 v) {
        float sx = sinf(v.x);
        float sy = sinf(v.y);
        float sz = sinf(v.z);
        float cx = cosf(v.x);
        float cy = cosf(v.y);
        float cz = cosf(v.z);

        xx = cy * cz;
        xy = cy * sz;
        xz = -sy;
        yx = sx * sy * cz - cx * sz;
        yy = sx * sy * sz + cx * cz;
        yz = sx * cy;
        zx = cx * sy * cz + sx * sz;
        zy = cx * sy * sz - sx * cz;
        zz = cx * cy;
    }

    void SetCamera(float roll, Vector3 position, Vector3 target) {
        Vector3 worldup(sinf(-roll), cosf(-roll), 0.0f);
        Vector3 view = -(target - position);
        //view.Negate();
        view.Normalize();
        float dot = worldup * view;
        Vector3 up = worldup - dot * view;
        up.Normalize();
        Vector3 right = up ^ view;

        Identity();
        xx = right.x;
        xy = up.x;
        xz = view.x;
        yx = right.y;
        yy = up.y;
        yz = view.y;
        zx = right.z;
        zy = up.z;
        zz = view.z;
        wx = -(position * right);
        wy = -(position * up);
        wz = -(position * view);

        //Inverse();
    }

    Matrix4 operator *= (const Matrix4& m) {
        *this = (*this) * m;
        return *this;
    }


};

inline Vector3& Vector3::operator *= (const Matrix4& m) {
    Vector3 r;
    r.x = x * m.xx + y * m.yx + z * m.zx + m.wx;
    r.y = x * m.xy + y * m.yy + z * m.zy + m.wy;
    r.z = x * m.xz + y * m.yz + z * m.zz + m.wz;
    *this = r;
    return *this;
}


inline Matrix4 operator*(const Matrix4& a, const Matrix4& b) {
    Matrix4 temp;

    temp.xx = a.xx * b.xx + a.yx * b.xy + a.zx * b.xz;
    temp.yx = a.xx * b.yx + a.yx * b.yy + a.zx * b.yz;
    temp.zx = a.xx * b.zx + a.yx * b.zy + a.zx * b.zz;
    temp.wx = a.xx * b.wx + a.yx * b.wy + a.zx * b.wz + a.wx;
    temp.xy = a.xy * b.xx + a.yy * b.xy + a.zy * b.xz;
    temp.yy = a.xy * b.yx + a.yy * b.yy + a.zy * b.yz;
    temp.zy = a.xy * b.zx + a.yy * b.zy + a.zy * b.zz;
    temp.wy = a.xy * b.wx + a.yy * b.wy + a.zy * b.wz + a.wy;
    temp.xz = a.xz * b.xx + a.yz * b.xy + a.zz * b.xz;
    temp.yz = a.xz * b.yx + a.yz * b.yy + a.zz * b.yz;
    temp.zz = a.xz * b.zx + a.yz * b.zy + a.zz * b.zz;
    temp.wz = a.xz * b.wx + a.yz * b.wy + a.zz * b.wz + a.wz;

    return temp;
}

struct Plane {
    Vector3 normal;
    float   distance;

    Plane() {}

    Plane(Vector3 const& v, float const& d) {
        normal.x = v.x;
        normal.y = v.y;
        normal.z = v.z;
        distance = d;
    }

    Plane(float ax, float ay, float az, float d) {
        normal.x = ax;
        normal.y = ay;
        normal.z = az;
        distance = d;
    }

    //beware of using this
    float& operator[](int ind) {
        return ((float*)&normal)[ind];
    }

    const float& operator[](int ind) const {
        return ((float*)&normal)[ind];
    }

    void Normalize() {
        float lng;
        lng = normal.Length();
        normal /= lng;
        distance /= lng;
    }
};

struct Sphere {
    Vector3 center;
    float   radius;
    float   radius_sqrd;    //used to speed up ray/sphere intersections

    Sphere() : center{ 0, 0, 0 }, radius{ 1.0f }, radius_sqrd{ 1.0f } {}

    void SetRadius(float r) {
        radius = fabsf(r);
        radius_sqrd = (float)(radius * radius);
    }

};

inline Vector3 CalcNormal(const Vector3& p, const Vector3& p1, const Vector3& p2) {
    return ((p1 - p2) ^ (p2 - p)).Normalized();
}

inline void Vector3::TransformNormal(const Matrix4& mat) {
    float x1 = x * mat[0] + y * mat[4] + z * mat[8];
    float y1 = x * mat[1] + y * mat[5] + z * mat[9];
    z = x * mat[2] + y * mat[6] + z * mat[10];
    x = x1;
    y = y1;
}

inline Vector3 Vector3::TransformedNormal(const Matrix4& mat) const {
    return Vector3(x * mat[0] + y * mat[4] + z * mat[8], x * mat[1] + y * mat[5] + z * mat[9], x * mat[2] + y * mat[6] + z * mat[10]);
}

#endif
