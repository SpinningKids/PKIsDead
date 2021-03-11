// Model.h: interface for the CModel class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MODEL_H
#define MODEL_H

#ifdef __WIN32__
#include <windows.h>
#endif /* __WIN32__ */
#include <GL/gl.h>
#include "Object.h"
#include "AsmMath4.h"
#include <assert.h>
#include "GLTexture.h"
#include "Timer.h"

#ifdef _MSC_VER
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#	define PATH_MAX _MAX_PATH
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#	include <limits.h>
#else
#	error you must byte-align these structures with the appropriate compiler directives
#endif

typedef unsigned char byte;
typedef unsigned short word;

// File header
struct MS3DHeader
{
	char m_ID[10];
	int m_version;
} PACK_STRUCT;

// Vertex information
struct MS3DVertex
{
	byte m_flags;
	Vector3 m_vertex;
	char m_boneID;
	byte m_refCount;
} PACK_STRUCT;

// Triangle information
struct MS3DTriangle
{
	word m_flags;
	word m_vertexIndices[3];
	float m_vertexNormals[3][3];
	float m_s[3], m_t[3];
	byte m_smoothingGroup;
	byte m_groupIndex;
} PACK_STRUCT;

// Material information
struct MS3DMaterial
{
    char m_name[32];
    float m_ambient[4];
    float m_diffuse[4];
    float m_specular[4];
    float m_emissive[4];
    float m_shininess;	// 0.0f - 128.0f
    float m_transparency;	// 0.0f - 1.0f
    byte m_mode;	// 0, 1, 2 is unused now
    char m_texture[128];
    char m_alphamap[128];
} PACK_STRUCT;

//	Joint information
struct MS3DJoint
{
	byte m_flags;
	char m_name[32];
	char m_parentName[32];
	Vector3 m_rotation;
	Vector3 m_translation;
	word m_numRotationKeyframes;
	word m_numTranslationKeyframes;
} PACK_STRUCT;

// Keyframe data
struct MS3DKeyframe
{
	float m_time;
	Vector3 m_parameter;
} PACK_STRUCT;


struct sPlaneEq
{
	float a,b,c,d;
};
//	Mesh
struct Mesh
{
	int m_materialIndex;
	int m_numTriangles;
	int *m_pTriangleIndices;
};

//	Triangle structure
struct Triangle
{
	Vector3	m_vertexNormals[3];
	float	m_s[3], m_t[3];
	int		m_vertexIndices[3];
	int		m_neigh[3];
	bool	m_visible;
	sPlaneEq	PlaneEq;
};

//	Vertex structure
struct Vertex
{
	char m_boneID;	// for skeletal animation
	Vector3 m_location;
};

class CModel : public CObject
{
public:
	//	Material properties
	struct Material
	{
		float m_ambient[4], m_diffuse[4], m_specular[4], m_emissive[4];
		float m_shininess;
		GLuint m_texture;
		char *m_pTextureFilename;
	};
	// { NEW }
	//	Animation keyframe information
	struct Keyframe
	{
		int m_jointIndex;
		float m_time;	// in milliseconds
		Vector3 m_parameter;
	};

	//	Skeleton bone joint
	struct Joint
	{
		//float m_localRotation[3];
		//float m_localTranslation[3];
		Vector3 m_localRotation;
		Vector3 m_localTranslation;
		Matrix4 m_absolute;
		Matrix4 m_relative;

		int m_numRotationKeyframes, m_numTranslationKeyframes;
		Keyframe *m_pTranslationKeyframes;
		Keyframe *m_pRotationKeyframes;

		int m_currentTranslationKeyframe, m_currentRotationKeyframe;
		Matrix4 m_final;

		int m_parent;
	};
	// { end NEW }


public:
	void PrepareArrays();
	void SetLooping(bool value);
	int GetActualFrame();
	void StopAnimation();
	void StartAnimation();
	int GetNumTris();
	void GetTriangle(int ntri,Vector3 *v1, Vector3 *v2, Vector3 *v3);
	void RestartAnimation();
	void SetNumTriangles(int num);
	void SetNumVertex(int num);
	void CalcShadowMatrix(Vector3 light);
	//void SetScale(float x, float y, float z);
	//Vector3 GetRotation();
	//Vector3 GetPosition();
	//void SetRotation(float x,float y, float z);
	//void SetPosition(float x,float y,float z);
	void CalcPlane(int tri);
	void CastShadow(int softness);
	void SetConnectivity();
	void Draw(float time);
	bool LoadData( int resnum, bool frommem);
	CModel();
	virtual ~CModel();

private:
	bool reloadTextures(GLTexture* tex);
	void advanceAnimation();
	void SetupJoints();
	void SetJointKeyframe( int jointIndex, int keyframeIndex, float time, float *parameter, bool isRotation );

protected:
	//light position, used for shadow calculus
	Vector3	lp;

private:
		//	Meshes used
		int m_numMeshes;
		Mesh *m_pMeshes;

		//	Materials used
		int m_numMaterials;
		Material *m_pMaterials;

		//	Triangles used
		int m_numTriangles;
		Triangle *m_pTriangles;

		//	Vertices Used
		int m_numVertices;
		Vertex *m_pVertices;

		//	Joint information
		int m_numJoints;
		Joint *m_pJoints;

		//	Total animation time
		double m_totalTime;

		//	Is the animation looping?
		bool m_looping;

		//	Timer variable
		Timer* m_pTimer;

    Vector3*  m_pv3VertArray;
    uv_coord* m_pfTexCoords;
    int*      m_piIndexes;
		int       m_numVerticesArray;
};

#endif // !defined(AFX_MODEL_H__CA695081_ADD0_11D5_A9C4_0040054FF84A__INCLUDED_)
