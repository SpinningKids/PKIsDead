// Object.cpp: implementation of the CObject class.
//
//////////////////////////////////////////////////////////////////////

#include "Object.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObject::CObject()
{
  SetPosition(0.0f,0.0f,0.0f);

  SetRotation(0.0f,0.0f,0.0f);

  SetScale(1.0f,1.0f,1.0f);

  m_fSpeed = 1;
  m_fFXSpeed = 1000;

}

CObject::~CObject()
{

}

void CObject::SetPosition(const Vector3* v)
{
  m_v3Position.x = v->x;
  m_v3Position.y = v->y;
  m_v3Position.z = v->z;
}

void CObject::SetPosition(float x, float y, float z)
{
  m_v3Position.x = x;
  m_v3Position.y = y;
  m_v3Position.z = z;
}

Vector3 CObject::GetPosition()
{
  return m_v3Position;
}

void CObject::SetRotation(const Vector3 *v)
{
  m_v3Rotation.x = v->x;
  m_v3Rotation.y = v->y;
  m_v3Rotation.z = v->z;
}

void CObject::SetRotation(float x, float y, float z)
{
  m_v3Rotation.x = x;
  m_v3Rotation.y = y;
  m_v3Rotation.z = z;
}

Vector3 CObject::GetRotation()
{
  return m_v3Rotation;
}

bool CObject::LoadData(const char *filename, bool frommem)
{
  //Generic load data function
  //used to retrieve object geometry/params
  return true;
}

void CObject::Draw(float time)
{
  //Generic drawing routine
  //Materials say if to render with proper materials or not
}

void CObject::SetScale(const Vector3 *v)
{
  m_v3Scale.x = v->x;
  m_v3Scale.y = v->y;
  m_v3Scale.z = v->z;
}

void CObject::SetScale(float x, float y, float z)
{
  m_v3Scale.x = x;
  m_v3Scale.y = y;
  m_v3Scale.z = z;
}

Vector3 CObject::GetScale()
{
  return m_v3Scale;
}

//VIRTUAL (Animation steps differs from object types)
int CObject::GetActualFrame()
{
  //generic function to get actual frame
  return 0;
}

void CObject::Update()
{

}

void CObject::CalcShadowMatrix(void *light)
{

}

void CObject::CastShadow(int i)
{

}


void CObject::Create()
{

}

void CObject::SetFXSpeed(float speed)
{
  m_fFXSpeed = speed;
}
