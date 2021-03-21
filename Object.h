// Object.h: interface for the CObject class.
//
//////////////////////////////////////////////////////////////////////

#ifndef OBJECT_H_
#define OBJECT_H_

#include "AsmMath4.h"

#define ENV_MAPPED_NONE     0
#define ENV_MAPPED_SPHERE   1
#define ENV_MAPPED_CUBE     2


class CObject
{
public:
    void SetFXSpeed(float speed);
    virtual void Create();
    void SetLOD(int newlod);
    virtual void CastShadow(int i);
    virtual void CalcShadowMatrix(void* light);
    void SetTextureEnvy(int val);
    void SetTexture2(int val);
    void SetTexture1(int val);
    float GetAlpha();
    void SetAlpha(float value);
    void SetEnvMapped(bool e);
    bool IsEnvMapped();

    //anim params
    virtual void PostOBJMessage(char* msg, float p1, float p2, float p3, float p4);
    virtual void Update();
    virtual void StopAnimation();
    virtual void RestartAnimation();
    virtual int GetActualFrame();
    virtual void StartAnimation();
    bool IsAnimated();

    //geom params
    Vector3 GetScale();
    void SetScale(float x, float y, float z);
    void SetScale(const Vector3* v);

    //draw funtions
    virtual void Draw(float time);

    //data functions
    virtual bool LoadData(const char* filename, bool frommem);
    Vector3 GetRotation();
    void SetRotation(float x, float y, float z);
    void SetRotation(const Vector3* v);
    void SetPosition(float x, float y, float z);
    Vector3 GetPosition();
    void SetPosition(const Vector3* v);
    CObject();
    virtual ~CObject();

protected:
    Vector3   m_v3Position;
    Vector3   m_v3Rotation;
    Vector3	  m_v3Scale;

    int       m_iTexture1ID;
    int       m_iTexture2ID;

    float     m_fAlpha;

    //indicates if the objet is in animation mode
    bool	m_bAnimated;

    //indicates if the object need to be drawn with sphere envy
    bool	m_bEnvMapped;
    int   m_iEnvyTexID;

    int m_iLod;

    float m_fSpeed;
    float m_fFXSpeed;

};

#endif // OBJECT_H_
