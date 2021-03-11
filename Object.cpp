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

	m_bAnimated = true;

  m_fAlpha = 1.0f;

  m_iTexture1ID = 0;
  m_iTexture2ID = 0;
  m_iEnvyTexID  = 0;

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

//NON VIRTUAL
bool CObject::IsAnimated()
{
  return m_bAnimated;
}

//VIRTUAL (Animation steps differs from object types)
int CObject::GetActualFrame()
{
  //generic function to get actual frame
  return 0;
}

//VIRTUAL (Animation steps differs from object types)
void CObject::StartAnimation()
{
  //Generic used to start animation
  m_bAnimated = true;
}

//VIRTUAL (Animation steps differs from object types)
void CObject::RestartAnimation()
{
  //Generic function to restart the object's animation
  m_bAnimated = true;
}

//VIRTUAL (Animation steps differs from object types)
void CObject::StopAnimation()
{
  //Generic function to stop object's animation
  m_bAnimated = false;
}

//NON VIRTUAL
bool CObject::IsEnvMapped()
{
  //Return EnvMapping abilitation
  return m_bEnvMapped;
}

//NON VIRTUAL
void CObject::SetEnvMapped(bool e)
{
  //Sets the Env mapping flag
  m_bEnvMapped = e;
}

void CObject::SetAlpha(float value)
{
  m_fAlpha = value;
}

float CObject::GetAlpha()
{
  return m_fAlpha;
}

void CObject::SetTexture1(int val)
{
  m_iTexture1ID = val;
}

void CObject::SetTexture2(int val)
{
  m_iTexture2ID = val;
}

void CObject::SetTextureEnvy(int val)
{
  m_iEnvyTexID = val;
}

void CObject::Update()
{

}

//generic message post
void CObject::PostOBJMessage(char *msg, float p1, float p2, float p3, float p4)
{

}

void CObject::CalcShadowMatrix(void *light)
{

}

void CObject::CastShadow(int i)
{

}


void CObject::SetLOD(int newlod)
{
  m_iLod = newlod;
}

void CObject::Create()
{

}

void CObject::SetFXSpeed(float speed)
{
  m_fFXSpeed = speed;
}
