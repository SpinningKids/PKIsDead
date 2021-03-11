// Frustum.h: interface for the CFrustum class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRUSTUM_H__67DA4703_2CA4_4DBB_B8E2_12817A8E10CF__INCLUDED_)
#define AFX_FRUSTUM_H__67DA4703_2CA4_4DBB_B8E2_12817A8E10CF__INCLUDED_

#ifdef __WIN32__
#include <windows.h>
#endif /* __WIN32__ */
#include <GL/gl.h>
#include "AsmMath4.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFrustum  
{
public:
	bool QuadInFrustum(Vector3 a, Vector3 b, Vector3 c, Vector3 d);
	bool SphereInFrustum(Sphere s);
	bool PointInFrustum(Vector3 p, float dist);
	bool TriangleInFrustum(Vector3 a,Vector3 b, Vector3 c);
	bool PointInFrustum(Vector3 p);
	void GetOGLFrustum();
	CFrustum();
	virtual ~CFrustum();

private:
  //the frustum planes
  Plane frustum[6];


};

#endif // !defined(AFX_FRUSTUM_H__67DA4703_2CA4_4DBB_B8E2_12817A8E10CF__INCLUDED_)
