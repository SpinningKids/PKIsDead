// Frustum.cpp: implementation of the CFrustum class.
//
//////////////////////////////////////////////////////////////////////

#include "Frustum.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFrustum::CFrustum()
{

}

CFrustum::~CFrustum()
{

}

void CFrustum::GetOGLFrustum()
{
  Matrix4 proj;
  Matrix4 modl;
  //float   proj[16];
  //float   modl[16];
  float   clip[16];
  float   t;

  // Get the current PROJECTION matrix
  glGetFloatv( GL_PROJECTION_MATRIX, proj.Get() );
  // Get the current MODELVIEW matrix
  glGetFloatv( GL_MODELVIEW_MATRIX, modl.Get() );

  // Combine the two matrices (multiply projection by modelview)
  clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
  clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
  clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
  clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

  clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
  clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
  clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
  clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

  clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
  clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
  clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
  clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

  clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
  clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
  clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
  clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

  // Extract the numbers for the RIGHT plane 
  frustum[0][0] = clip[ 3] - clip[ 0];
  frustum[0][1] = clip[ 7] - clip[ 4];
  frustum[0][2] = clip[11] - clip[ 8];
  frustum[0][3] = clip[15] - clip[12];

  // Normalize the result 
/*  t = sqrt( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2] );
  frustum[0][0] /= t;
  frustum[0][1] /= t;
  frustum[0][2] /= t;
  frustum[0][3] /= t;
*/
  frustum[0].Normalize();

  // Extract the numbers for the LEFT plane
  frustum[1][0] = clip[ 3] + clip[ 0];
  frustum[1][1] = clip[ 7] + clip[ 4];
  frustum[1][2] = clip[11] + clip[ 8];
  frustum[1][3] = clip[15] + clip[12];

  // Normalize the result 
/*  t = sqrt( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2] );
  frustum[1][0] /= t;
  frustum[1][1] /= t;
  frustum[1][2] /= t;
  frustum[1][3] /= t;
*/
  frustum[1].Normalize();

  // Extract the BOTTOM plane 
  frustum[2][0] = clip[ 3] + clip[ 1];
  frustum[2][1] = clip[ 7] + clip[ 5];
  frustum[2][2] = clip[11] + clip[ 9];
  frustum[2][3] = clip[15] + clip[13];

  // Normalize the result
/*  t = sqrt( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2] );
  frustum[2][0] /= t;
  frustum[2][1] /= t;
  frustum[2][2] /= t;
  frustum[2][3] /= t;
*/
  frustum[2].Normalize();

  // Extract the TOP plane
  frustum[3][0] = clip[ 3] - clip[ 1];
  frustum[3][1] = clip[ 7] - clip[ 5];
  frustum[3][2] = clip[11] - clip[ 9];
  frustum[3][3] = clip[15] - clip[13];

  // Normalize the result 
/*  t = sqrt( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2] );
  frustum[3][0] /= t;
  frustum[3][1] /= t;
  frustum[3][2] /= t;
  frustum[3][3] /= t;
*/
  frustum[3].Normalize();

  // Extract the FAR plane 
  frustum[4][0] = clip[ 3] - clip[ 2];
  frustum[4][1] = clip[ 7] - clip[ 6];
  frustum[4][2] = clip[11] - clip[10];
  frustum[4][3] = clip[15] - clip[14];

  // Normalize the result
/*  t = sqrt( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2] );
  frustum[4][0] /= t;
  frustum[4][1] /= t;
  frustum[4][2] /= t;
  frustum[4][3] /= t;
*/
  frustum[4].Normalize();

  // Extract the NEAR plane
  frustum[5][0] = clip[ 3] + clip[ 2];
  frustum[5][1] = clip[ 7] + clip[ 6];
  frustum[5][2] = clip[11] + clip[10];
  frustum[5][3] = clip[15] + clip[14];

  // Normalize the result
/*  t = sqrt( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2] );
  frustum[5][0] /= t;
  frustum[5][1] /= t;
  frustum[5][2] /= t;
  frustum[5][3] /= t;
*/
  frustum[5].Normalize();

}

bool CFrustum::PointInFrustum(Vector3 p)
{

  int np;

  for( np = 0; np < 6; np++ )
  {
    if( (frustum[np].normal.x * p.x) + 
        (frustum[np].normal.y * p.y) + 
        (frustum[np].normal.z * p.z) + 
        frustum[np].distance <= 0 )
       return false;
  }

  return true;

}

bool CFrustum::PointInFrustum(Vector3 p, float dist)
{

  int np;

  for( np = 0; np < 6; np++ )
  {
    if( (frustum[np].normal.x * p.x) + 
        (frustum[np].normal.y * p.y) + 
        (frustum[np].normal.z * p.z) + 
        frustum[np].distance <= -dist )
       return false;
  }

  return true;

}

bool CFrustum::TriangleInFrustum(Vector3 a, Vector3 b, Vector3 c)
{
  if (!PointInFrustum(a) && !PointInFrustum(b) && !PointInFrustum(c))
    return false;

  return true;
}


bool CFrustum::SphereInFrustum(Sphere s)
{
  if (!PointInFrustum(s.center,s.radius))
    return false;

  return true;
}

bool CFrustum::QuadInFrustum(Vector3 a, Vector3 b, Vector3 c, Vector3 d)
{
  if (!PointInFrustum(a) && !PointInFrustum(b) && !PointInFrustum(c) && !PointInFrustum(d))
    return false;

  return true;

}
