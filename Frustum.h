// Frustum.h: interface for the CFrustum class.
//
//////////////////////////////////////////////////////////////////////

#ifndef FRUSTUM_H_
#define FRUSTUM_H_

#include "AsmMath4.h"

class CFrustum
{
public:
    bool QuadInFrustum(Vector3 a, Vector3 b, Vector3 c, Vector3 d);
    bool SphereInFrustum(Sphere s);
    bool PointInFrustum(Vector3 p, float dist);
    bool TriangleInFrustum(Vector3 a, Vector3 b, Vector3 c);
    bool PointInFrustum(Vector3 p);
    void GetOGLFrustum();
    CFrustum();
    virtual ~CFrustum();

private:
    //the frustum planes
    Plane frustum[6];
};

#endif // FRUSTUM_H_
