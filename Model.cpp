// Model.cpp: implementation of the CModel class.
//
//////////////////////////////////////////////////////////////////////
//#include "SpinEngine.h"
#include "Model.h"
#include <fstream.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModel::CModel()
{
	m_numMeshes = 0;
	m_pMeshes = NULL;
	m_numMaterials = 0;
	m_pMaterials = NULL;
	m_numTriangles = 0;
	m_pTriangles = NULL;
	m_numVertices = 0;
	m_pVertices = NULL;
	m_numJoints = 0;
	m_pJoints = NULL;

	m_pTimer = (Timer*)Timer::getNewTimer();

	m_looping = true;

	RestartAnimation();

}

CModel::~CModel()
{
	int i;
	for ( i = 0; i < m_numMeshes; i++ )
		delete[] m_pMeshes[i].m_pTriangleIndices;
	for ( i = 0; i < m_numMaterials; i++ )
		delete[] m_pMaterials[i].m_pTextureFilename;

	m_numMeshes = 0;
	if ( m_pMeshes != NULL )
	{
		delete[] m_pMeshes;
		m_pMeshes = NULL;
	}

	m_numMaterials = 0;
	if ( m_pMaterials != NULL )
	{
		delete[] m_pMaterials;
		m_pMaterials = NULL;
	}

	m_numTriangles = 0;
	if ( m_pTriangles != NULL )
	{
		delete[] m_pTriangles;
		m_pTriangles = NULL;
	}

	m_numVertices = 0;
	if ( m_pVertices != NULL )
	{
		delete[] m_pVertices;
		m_pVertices = NULL;
	}

	// { NEW }
	for ( i = 0; i < m_numJoints; i++ )
	{
		delete[] m_pJoints[i].m_pRotationKeyframes;
		delete[] m_pJoints[i].m_pTranslationKeyframes;
	}

	m_numJoints = 0;
	if ( m_pJoints != NULL )
	{
		delete[] m_pJoints;
		m_pJoints = NULL;
	}

  delete [] m_pv3VertArray;
  delete [] m_pfTexCoords;

	delete[] m_pTimer;
}

bool CModel::LoadData(int resnum, bool frommem)
{
  const byte *pPtr;
  void *pBuffer;
	char pathTemp[_MAX_PATH+1];
	int pathLength;
  int i,u;
  HRSRC           hsResourceFH;             /* needed to load resources     */
  HGLOBAL         hgResourceLH;
  HINSTANCE	      hInstance  = GetModuleHandleA(NULL); 
  int size = 0;

  //assert(1 == 0);

  hsResourceFH = FindResource(hInstance, MAKEINTRESOURCE(resnum), "MS3DRES1");
  if(hsResourceFH)
  {
    hgResourceLH = LoadResource(hInstance, hsResourceFH);
    if(hgResourceLH)
    {
       size = SizeofResource(hInstance, hsResourceFH);
       pBuffer = (void*)LockResource(hgResourceLH);
       //pPtr = new byte[size];
    }
  } 
  //CPacker packer;
  //packer.GetFromMemory = false;
	
  //pBuffer = (unsigned char*)packer.GetPointerFor(filename);
  //pPtr = new byte[packer.GetLastSize()];
  //assert( 1 == 0);
  pPtr = (unsigned char*)pBuffer;

  //MS3D 1.5 wants textures in relative path
  //if frommem relative path is empty
  strcpy( pathTemp, "" );
  pathLength = 0;
  

	MS3DHeader *pHeader = ( MS3DHeader* )pPtr;
	pPtr += sizeof( MS3DHeader );

	if ( strncmp( pHeader->m_ID, "MS3D000000", 10 ) != 0 )
		return false; // "Not a valid Milkshape3D model file."

	if ( pHeader->m_version < 3 || pHeader->m_version > 4 )
		return false; // "Unhandled file version. Only Milkshape3D Version 1.3 and 1.4 is supported." );

	int nVertices = *( word* )pPtr; 
	m_numVertices = nVertices;
	m_pVertices = new Vertex[nVertices];
	pPtr += sizeof( word );

	for ( i = 0; i < nVertices; i++ )
	{
		MS3DVertex *pVertex = ( MS3DVertex* )pPtr;
		m_pVertices[i].m_boneID = pVertex->m_boneID;
		float tmp[3];
		memcpy((void *)&tmp[0], (void *)&pVertex->m_vertex, sizeof( float )*3 );
		m_pVertices[i].m_location.x = tmp[0];
		m_pVertices[i].m_location.y = tmp[1];
		m_pVertices[i].m_location.z = tmp[2];
		pPtr += sizeof( MS3DVertex );
	}

	int nTriangles = *( word* )pPtr;
	m_numTriangles = nTriangles;
	m_pTriangles = new Triangle[nTriangles];
	pPtr += sizeof( word );

	for ( i = 0; i < nTriangles; i++ )
	{
		MS3DTriangle *pTriangle = ( MS3DTriangle* )pPtr;
		int vertexIndices[3] = { pTriangle->m_vertexIndices[0], pTriangle->m_vertexIndices[1], pTriangle->m_vertexIndices[2] };
		float t[3] = { 1.0f-pTriangle->m_t[0], 1.0f-pTriangle->m_t[1], 1.0f-pTriangle->m_t[2] };
		memcpy( m_pTriangles[i].m_vertexNormals, pTriangle->m_vertexNormals, sizeof( float )*3*3 );
		memcpy( m_pTriangles[i].m_s, pTriangle->m_s, sizeof( float )*3 );
		memcpy( m_pTriangles[i].m_t, t, sizeof( float )*3 );
		memcpy( m_pTriangles[i].m_vertexIndices, vertexIndices, sizeof( int )*3 );
    //set no connectivity data
    m_pTriangles[i].m_neigh[0] = 0;
    m_pTriangles[i].m_neigh[1] = 0;
    m_pTriangles[i].m_neigh[2] = 0;
		pPtr += sizeof( MS3DTriangle );
	}

	int nGroups = *( word* )pPtr;
	m_numMeshes = nGroups;
	m_pMeshes = new Mesh[nGroups];
	pPtr += sizeof( word );
	for ( i = 0; i < nGroups; i++ )
	{
		pPtr += sizeof( byte );	// flags
		pPtr += 32;				// name

		word nTriangles = *( word* )pPtr;
		pPtr += sizeof( word );
		int *pTriangleIndices = new int[nTriangles];
		for ( int j = 0; j < nTriangles; j++ )
		{
			pTriangleIndices[j] = *( word* )pPtr;
			pPtr += sizeof( word );
		}

		char materialIndex = *( char* )pPtr;
		pPtr += sizeof( char );
	
		m_pMeshes[i].m_materialIndex = materialIndex;
		m_pMeshes[i].m_numTriangles = nTriangles;
		m_pMeshes[i].m_pTriangleIndices = pTriangleIndices;
	}

	int nMaterials = *( word* )pPtr;
	m_numMaterials = nMaterials;
	m_pMaterials = new Material[nMaterials];
	pPtr += sizeof( word );
	for ( i = 0; i < nMaterials; i++ )
	{
		MS3DMaterial *pMaterial = ( MS3DMaterial* )pPtr;
		memcpy( m_pMaterials[i].m_ambient, pMaterial->m_ambient, sizeof( float )*4 );
		memcpy( m_pMaterials[i].m_diffuse, pMaterial->m_diffuse, sizeof( float )*4 );
		memcpy( m_pMaterials[i].m_specular, pMaterial->m_specular, sizeof( float )*4 );
		memcpy( m_pMaterials[i].m_emissive, pMaterial->m_emissive, sizeof( float )*4 );
		m_pMaterials[i].m_shininess = pMaterial->m_shininess;
		if ( strncmp( pMaterial->m_texture, ".\\", 2 ) == 0 ) {
			// MS3D 1.5.x relative path
      if (frommem == false)
			  strcpy( pathTemp + pathLength, pMaterial->m_texture + 1 );
      else
			  strcpy( pathTemp + pathLength, pMaterial->m_texture + 2 );
        
			m_pMaterials[i].m_pTextureFilename = new char[strlen( pathTemp )+1];
			strcpy( m_pMaterials[i].m_pTextureFilename, pathTemp );
		}
		else {
			// MS3D 1.4.x or earlier - absolute path
			m_pMaterials[i].m_pTextureFilename = new char[strlen( pMaterial->m_texture )+1];
			strcpy( m_pMaterials[i].m_pTextureFilename, pMaterial->m_texture );
		}
		pPtr += sizeof( MS3DMaterial );
	}

  //assert( 1 ==0);
	reloadTextures(NULL);

// { NEW }
	// Load Skeletal Animation Stuff

	float animFPS = *( float* )pPtr;
	pPtr += sizeof( float );

	// skip currentTime
	pPtr += sizeof( float );

	int totalFrames = *( int* )pPtr;
	pPtr += sizeof( int );

	m_totalTime = totalFrames*1000.0/animFPS;

	m_numJoints = *( word* )pPtr;
	pPtr += sizeof( word );

	m_pJoints = new Joint[m_numJoints];

  //zero some memory to make the joints vars values to 0
  ZeroMemory(m_pJoints,sizeof(Joint) * m_numJoints);

	struct JointNameListRec
	{
		int m_jointIndex;
		const char *m_pName;
	};

	const byte *pTempPtr = pPtr;

	JointNameListRec *pNameList = new JointNameListRec[m_numJoints];
	for ( i = 0; i < m_numJoints; i++ )
	{
		MS3DJoint *pJoint = ( MS3DJoint* )pTempPtr;
		pTempPtr += sizeof( MS3DJoint );
		pTempPtr += sizeof( MS3DKeyframe )*( pJoint->m_numRotationKeyframes+pJoint->m_numTranslationKeyframes );

		pNameList[i].m_jointIndex = i;
		pNameList[i].m_pName = pJoint->m_name;
	}

	for ( i = 0; i < m_numJoints; i++ )
	{
		MS3DJoint *pJoint = ( MS3DJoint* )pPtr;
		pPtr += sizeof( MS3DJoint );

		int j, parentIndex = -1;
		if ( strlen( pJoint->m_parentName ) > 0 )
		{
			for ( j = 0; j < m_numJoints; j++ )
			{
				if ( stricmp( pNameList[j].m_pName, pJoint->m_parentName ) == 0 )
				{
					parentIndex = pNameList[j].m_jointIndex;
					break;
				}
			}
			if ( parentIndex == -1 ) // Unable to find parent bone in MS3D file
				return false;
		}

		m_pJoints[i].m_localRotation = pJoint->m_rotation;
		m_pJoints[i].m_localTranslation = pJoint->m_translation;
		//memcpy( (void *)&(m_pJoints[i].m_localRotation), pJoint->m_rotation, sizeof( float )*3 );
		//memcpy( (void *)&(m_pJoints[i].m_localTranslation), pJoint->m_translation, sizeof( float )*3 );
		m_pJoints[i].m_parent = parentIndex;
		m_pJoints[i].m_numRotationKeyframes = pJoint->m_numRotationKeyframes;
		m_pJoints[i].m_pRotationKeyframes = new Keyframe[pJoint->m_numRotationKeyframes];
		m_pJoints[i].m_numTranslationKeyframes = pJoint->m_numTranslationKeyframes;
		m_pJoints[i].m_pTranslationKeyframes = new Keyframe[pJoint->m_numTranslationKeyframes];

		for ( j = 0; j < pJoint->m_numRotationKeyframes; j++ )
		{
			MS3DKeyframe *pKeyframe = ( MS3DKeyframe* )pPtr;
			pPtr += sizeof( MS3DKeyframe );

			SetJointKeyframe( i, j, pKeyframe->m_time*1000.0f, (float*)&pKeyframe->m_parameter, true );
		}

		for ( j = 0; j < pJoint->m_numTranslationKeyframes; j++ )
		{
			MS3DKeyframe *pKeyframe = ( MS3DKeyframe* )pPtr;
			pPtr += sizeof( MS3DKeyframe );

			SetJointKeyframe( i, j, pKeyframe->m_time*1000.0f, (float*)&pKeyframe->m_parameter, false );
		}
	}
	delete[] pNameList;

	SetupJoints();
// { end NEW }
  //can't delete a resource !! Idiot!! (rIO)
//	delete[] pBuffer;

  //assert(1 == 0);
  reloadTextures(NULL);

  //####################################################à
  //allocate mem for vertexarays
  m_pv3VertArray = (Vector3*) new Vector3[m_numVertices];
  for (i = 0; i<m_numVertices; i++)
    m_pv3VertArray[i] = m_pVertices[i].m_location;

  m_pfTexCoords = (uv_coord*) new uv_coord[m_numTriangles * 3 * 2];
  for (i = 0; i<m_numTriangles; i++)
  {
    for (u = 0; u < 3;u++)
    {
      m_pfTexCoords[m_pTriangles[i].m_vertexIndices[u]].u = m_pTriangles[i].m_s[u];
      m_pfTexCoords[m_pTriangles[i].m_vertexIndices[u]].v = m_pTriangles[i].m_t[u];
    }
  }

  //int*      m_piIndexes;
  m_piIndexes = (int*) new int[m_numTriangles * 3];

  //####################################################à

  SetConnectivity();

	return true;
}

bool CModel::reloadTextures(GLTexture* tex)
{
	for ( int i = 0; i < m_numMaterials; i++ )
		if ( strlen( m_pMaterials[i].m_pTextureFilename ) > 0 )
			m_pMaterials[i].m_texture = tex->getID();
		else
			m_pMaterials[i].m_texture = 0;

	return true;
}

void CModel::Draw(float time)
{

  PrepareArrays();

//	GLboolean texEnabled = glIsEnabled( GL_TEXTURE_2D );
//	GLboolean envEnabled = glIsEnabled( GL_TEXTURE_GEN_S );

	glPushMatrix();
	glTranslatef(m_v3Position.x,m_v3Position.y,m_v3Position.z);
	glRotatef(m_v3Rotation.x,1,0,0);
	glRotatef(m_v3Rotation.y,0,1,0);
	glRotatef(m_v3Rotation.z,0,0,1);
	glScalef(m_v3Scale.x,m_v3Scale.y,m_v3Scale.z);
/*
  if (m_fAlpha < 1.0f)
  {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
  }
  else
  {
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
  }
*/

  //begin shadowing optimization test

/*  if (m_pMaterials[0].m_texture.ID > 0)
    m_pMaterials[0].m_texture.Bind2D();

  glBegin(GL_TRIANGLES);
  for (int i = 0; i< m_numTriangles * 3 ; i++)
  {
    glTexCoord2f(m_pfTexCoords[m_piIndexes[i]].u,m_pfTexCoords[m_piIndexes[i]].v);
    glVertex3f(m_pv3VertArray[m_piIndexes[i]].x,m_pv3VertArray[m_piIndexes[i]].y,m_pv3VertArray[m_piIndexes[i]].z);

  }
  glEnd();
*/

	// Draw by group
	for ( int i = 0; i < m_numMeshes; i++ )
	{
/*
		int materialIndex = m_pMeshes[i].m_materialIndex;
		if ( materialIndex >= 0 )
		{
			glMaterialfv( GL_FRONT, GL_AMBIENT, m_pMaterials[materialIndex].m_ambient );
			glMaterialfv( GL_FRONT, GL_DIFFUSE, m_pMaterials[materialIndex].m_diffuse );
			glMaterialfv( GL_FRONT, GL_SPECULAR, m_pMaterials[materialIndex].m_specular );
			glMaterialfv( GL_FRONT, GL_EMISSION, m_pMaterials[materialIndex].m_emissive );
			glMaterialf( GL_FRONT, GL_SHININESS, m_pMaterials[materialIndex].m_shininess );

			if ( m_pMaterials[materialIndex].m_texture > 0 )
			{
				glBindTexture(GL_TEXTURE_2D,m_pMaterials[materialIndex].m_texture);
				glEnable( GL_TEXTURE_2D );
			}
			else
				glDisable( GL_TEXTURE_2D );
		}
		else
		{
			// Material properties?
			glDisable( GL_TEXTURE_2D );
		}
*/
		glBegin( GL_TRIANGLES );
		{
/*      //if is alpha enabled
      if (m_fAlpha < 1.0f)
        glColor4f(1.0f,1.0f,1.0f,m_fAlpha);
      else
        glColor3f(1.0f,1.0f,1.0f);
*/
			for ( int j = 0; j < m_pMeshes[i].m_numTriangles; j++ )
			{
				int triangleIndex = m_pMeshes[i].m_pTriangleIndices[j];
				const Triangle* pTri = &m_pTriangles[triangleIndex];

				for ( int k = 0; k < 3; k++ )
				{
					int index = pTri->m_vertexIndices[k];

          //glTexCoord2f(m_pfTexCoords[index].u,m_pfTexCoords[index].v);
					if ( m_pVertices[index].m_boneID == -1 )
					{
						glNormal3f(pTri->m_vertexNormals[k].x,pTri->m_vertexNormals[k].y,pTri->m_vertexNormals[k].z);
          }
          else
          {
						const Matrix4& final = m_pJoints[m_pVertices[index].m_boneID].m_final;
						Vector3 newNormal( pTri->m_vertexNormals[k] );
						newNormal.Transform3( (float*)&final );
						newNormal.Normalize();
						glNormal3fv((float*)&newNormal);
          }
          glVertex3f(m_pv3VertArray[index].x,m_pv3VertArray[index].y,m_pv3VertArray[index].z);
					// { NEW }
/*					if ( m_pVertices[index].m_boneID == -1 )
					{
						glTexCoord2f( pTri->m_s[k], pTri->m_t[k] );
						glNormal3f(pTri->m_vertexNormals[k].x,pTri->m_vertexNormals[k].y,pTri->m_vertexNormals[k].z);
						glVertex3f( m_pVertices[index].m_location.x,m_pVertices[index].m_location.y,m_pVertices[index].m_location.z );
					}
					else
					{
						// rotate according to transformation matrix
						const Matrix4& final = m_pJoints[m_pVertices[index].m_boneID].m_final;

						glTexCoord2f( pTri->m_s[k], pTri->m_t[k] );

						Vector3 newNormal( pTri->m_vertexNormals[k] );
						newNormal.Transform3( (float*)&final );
						newNormal.Normalize();
						glNormal3fv((float*)&newNormal);

						Vector3 newVertex(m_pVertices[index].m_location.x, m_pVertices[index].m_location.y, m_pVertices[index].m_location.z);
						newVertex.Transform( (float*)&final );
						glVertex3f(newVertex.x,newVertex.y,newVertex.z);
					}
*/
					// { end NEW }
				}
			}
		}
		glEnd();
	}

/*
	glPopMatrix();

	if ( texEnabled )
		glEnable( GL_TEXTURE_2D );
	else
		glDisable( GL_TEXTURE_2D );
*/
}

//used for shadows and toon rendering..
void CModel::SetConnectivity()
{
	unsigned int p1i, p2i, p1j, p2j;
	unsigned int P1i, P2i, P1j, P2j;
	int i,j,ki,kj;

	for(i=0;i<this->m_numTriangles-1; i++)
	{
		for(j=i+1;j<this->m_numTriangles;j++)
		{
			for(ki=0;ki<3;ki++)
			{
				if(!this->m_pTriangles[i].m_neigh[ki])
				{
					for(kj=0;kj<3;kj++)
					{
						p1i=ki;
						p1j=kj;
						p2i=(ki+1)%3;
						p2j=(kj+1)%3;

						p1i = this->m_pTriangles[i].m_vertexIndices[p1i];
						p2i = this->m_pTriangles[i].m_vertexIndices[p2i];
						p1j = this->m_pTriangles[j].m_vertexIndices[p1j];
						p2j = this->m_pTriangles[j].m_vertexIndices[p2j];

						
						P1i=((p1i+p2i)-abs(p1i-p2i))/2;
						P2i=((p1i+p2i)+abs(p1i-p2i))/2;
						P1j=((p1j+p2j)-abs(p1j-p2j))/2;
						P2j=((p1j+p2j)+abs(p1j-p2j))/2;

						//sono vicini
						if((P1i==P1j) && (P2i==P2j))
						{
							this->m_pTriangles[i].m_neigh[ki] = j+1;
							this->m_pTriangles[j].m_neigh[kj] = i+1;
						}
					}
				}
			}
		}
	}
	
	//Calc plane equation for all triangles
	for (int nt = 0;nt < this->m_numTriangles ;nt++)
	{
		CalcPlane(nt);				// Compute Plane Equations For All Faces
	}

}

void CModel::CastShadow(int softness)
{
	int	i, j, k, jj;
	unsigned int	p1, p2;
	Vector3			v1, v2;
	float			side;

	//set visual parameter
	for (i=0;i<this->m_numTriangles ;i++)
	{
    //if animated must recalc planes for every frame
    if (m_bAnimated)
      CalcPlane(i);

		// chech to see if light is in front or behind the plane (face plane)
		side = this->m_pTriangles[i].PlaneEq.a * lp[0]+
				this->m_pTriangles[i].PlaneEq.b * lp[1]+
				this->m_pTriangles[i].PlaneEq.c * lp[2]+
				this->m_pTriangles[i].PlaneEq.d * 1; // got mad for this!! fuck! lp[3];

		if (side >0) 
			this->m_pTriangles[i].m_visible = true;
		else
			this->m_pTriangles[i].m_visible = false;
	}

	for (int nums = 0; nums <softness;nums++)
	{
		if (softness > 1)
		{
			lp[0] += nums * (4 / softness);// / softness;
			lp[1] -= nums * (4 / softness);// / softness;
		}

		glClear(GL_STENCIL_BUFFER_BIT);

		//Need to cull the stencil shadow volume!!!!!!!
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);

    glDisable(GL_BLEND);

    glDisable(GL_TEXTURE_2D);

    glEnable(GL_DEPTH_TEST);

 		glDisable(GL_LIGHTING);
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);
		
		glEnable(GL_STENCIL_TEST);
		glColorMask(0, 0, 0, 0);
		glStencilFunc(GL_ALWAYS, 1, 0xffffffff);

		// first pass, stencil operation decreases stencil value
		glFrontFace(GL_CCW);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

		glPushMatrix();
		glScalef(m_v3Scale.x,m_v3Scale.y,m_v3Scale.z);
		glTranslatef(m_v3Position.x,m_v3Position.y,m_v3Position.z);
		glRotatef(m_v3Rotation.x,1,0,0);
		glRotatef(m_v3Rotation.y,0,1,0);
		glRotatef(m_v3Rotation.z,0,0,1);
		for (i=0; i<this->m_numTriangles;i++)
		{
			if (this->m_pTriangles[i].m_visible)
				for (j=0;j<3;j++)
				{
					//k = o->planes[i].neigh[j];
					k = this->m_pTriangles[i].m_neigh[j];
					//if ((!k) || (!o->planes[k-1].visible))
					if ((!k) || (!this->m_pTriangles[k-1].m_visible))
					{
						// here we have an edge, we must draw a polygon
						//p1 = o->planes[i].p[j];
						p1 = m_pTriangles[i].m_vertexIndices[j];
						jj = (j+1)%3;
						//p2 = o->planes[i].p[jj];
						p2 = m_pTriangles[i].m_vertexIndices[jj];

						//calculate the length of the vector
						v1.x = (m_pv3VertArray[p1].x - lp[0])*300;
						v1.y = (m_pv3VertArray[p1].y - lp[1])*300;
						v1.z = (m_pv3VertArray[p1].z - lp[2])*300;

						v2.x = (m_pv3VertArray[p2].x - lp[0])*300;
						v2.y = (m_pv3VertArray[p2].y - lp[1])*300;
						v2.z = (m_pv3VertArray[p2].z - lp[2])*300;
						
						//draw the polygon
						glBegin(GL_TRIANGLE_STRIP);
							glVertex3fv((GLfloat*)&m_pv3VertArray[p1]);
							glVertex3f(m_pv3VertArray[p1].x + v1.x,
									   m_pv3VertArray[p1].y + v1.y,
									   m_pv3VertArray[p1].z + v1.z);

							glVertex3fv((GLfloat*)&m_pv3VertArray[p2]);
							glVertex3f(m_pv3VertArray[p2].x + v2.x,
										m_pv3VertArray[p2].y + v2.y,
										m_pv3VertArray[p2].z + v2.z);
						glEnd();
					}
				}
		}

		// second pass, stencil operation increases stencil value
		glFrontFace(GL_CW);
		glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
		for (i=0; i<this->m_numTriangles;i++)
		{
			if (m_pTriangles[i].m_visible)
				for (j=0;j<3;j++)
				{
					k = m_pTriangles[i].m_neigh[j];
					if ((!k) || (!m_pTriangles[k-1].m_visible)){
						// here we have an edge, we must draw a polygon
						//p1 = o->planes[i].p[j];
						p1 = m_pTriangles[i].m_vertexIndices[j];
						jj = (j+1)%3;
						p2 = m_pTriangles[i].m_vertexIndices[jj];

						//calculate the length of the vector
						v1.x = (m_pv3VertArray[p1].x - lp[0])*300;
						v1.y = (m_pv3VertArray[p1].y - lp[1])*300;
						v1.z = (m_pv3VertArray[p1].z - lp[2])*300;

						v2.x = (m_pv3VertArray[p2].x - lp[0])*300;
						v2.y = (m_pv3VertArray[p2].y - lp[1])*300;
						v2.z = (m_pv3VertArray[p2].z - lp[2])*300;
						
						//draw the polygon
						glBegin(GL_TRIANGLE_STRIP);
							glVertex3fv((GLfloat*)&m_pv3VertArray[p1]);
							glVertex3f(m_pv3VertArray[p1].x + v1.x,
										m_pv3VertArray[p1].y + v1.y,
										m_pv3VertArray[p1].z + v1.z);

							glVertex3fv((GLfloat*)&m_pv3VertArray[p2]);
							glVertex3f(m_pv3VertArray[p2].x + v2.x,
										m_pv3VertArray[p2].y + v2.y,
										m_pv3VertArray[p2].z + v2.z);
						glEnd();
					}
				}
		}

		glPopMatrix();

		glFrontFace(GL_CCW);
		glColorMask(1, 1, 1, 1);

		//draw a shadowing rectangle covering the entire screen
		glColor4f(0.0f, 0.0f, 0.0f, (0.2f / softness));
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glStencilFunc(GL_NOTEQUAL, 0, 0xffffffff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glPushMatrix();
		glLoadIdentity();
		glBegin(GL_TRIANGLE_STRIP);
			glVertex3f(-0.1f, 0.1f,-0.10f);
			glVertex3f(-0.1f,-0.1f,-0.10f);
			glVertex3f( 0.1f, 0.1f,-0.10f);
			glVertex3f( 0.1f,-0.1f,-0.10f);
		glEnd();
		glPopMatrix();
		glDisable(GL_BLEND);
	} //softnes loop

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glEnable(GL_LIGHTING);
	glDisable(GL_STENCIL_TEST);
	glShadeModel(GL_SMOOTH);

}


void CModel::CalcPlane(int tri)
{
	Vector3 v[4];
	int i;

	for (i=0;i<3;i++)
	{
		v[i+1] = m_pv3VertArray[m_pTriangles[tri].m_vertexIndices[i]];
		//v[i+1] = m_pVertices[m_pTriangles[tri].m_vertexIndices[i]].m_location;
	}

	m_pTriangles[tri].PlaneEq.a = v[1].y*(v[2].z-v[3].z) + v[2].y*(v[3].z-v[1].z) + v[3].y*(v[1].z-v[2].z);
	m_pTriangles[tri].PlaneEq.b = v[1].z*(v[2].x-v[3].x) + v[2].z*(v[3].x-v[1].x) + v[3].z*(v[1].x-v[2].x);
	m_pTriangles[tri].PlaneEq.c = v[1].x*(v[2].y-v[3].y) + v[2].x*(v[3].y-v[1].y) + v[3].x*(v[1].y-v[2].y);
	m_pTriangles[tri].PlaneEq.d =-( v[1].x*(v[2].y*v[3].z - v[3].y*v[2].z) +
					  v[2].x*(v[3].y*v[1].z - v[1].y*v[3].z) +
					  v[3].x*(v[1].y*v[2].z - v[2].y*v[1].z) );



}


/*

void CModel::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}

void CModel::SetRotation(float x, float y, float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
}

Vector3 CModel::GetPosition()
{
	return m_position;
}

Vector3 CModel::GetRotation()
{
	return m_rotation;
}

void CModel::SetScale(float x, float y, float z)
{
	m_scale.x = x;
	m_scale.y = y;
	m_scale.z = z;
}
*/
void CModel::CalcShadowMatrix(Vector3 plight)
{

	//GLmatrix16f Minv;
  Matrix4 Minv;
	Vector3 wlp;
	//We need to calculate light position relative to the actual amtrix
	//Let's try it...
  //Vector3 plight;

  //plight = light;
	// calculate light's position relative to local coordinate system
	// dunno if this is the best way to do it, but it actually works
	// if u find another aproach, let me know ;)

	// we build the inversed matrix by doing all the actions in reverse order
	// and with reverse parameters (notice -xrot, -yrot, -ObjPos[], etc.)
	lp[0] = 0;lp[1] = 0;lp[2] = 0;

	glPushMatrix();
	glLoadIdentity();									// Reset Matrix

	glRotatef(-m_v3Rotation.z,0,0,1);
	glRotatef(-m_v3Rotation.y,0,1,0);
	glRotatef(-m_v3Rotation.x,1,0,0);

	glGetFloatv(GL_MODELVIEW_MATRIX,(float*)&Minv);				// Retrieve ModelView Matrix (Stores In Minv)
	lp[0] = plight.x;
	lp[1] = plight.y;
	lp[2] = plight.z;
	//lp[3] = plight->GetPosition()[3];

  //Old Math3
  //VMatMult((float*)&Minv, (float*)&lp);									// We Store Rotated Light Vector In 'lp' Array
  //New AsmMath4
  lp *= Minv;

	glTranslatef(-m_v3Position.x,-m_v3Position.y,-m_v3Position.z);	// Move Negative On All Axis Based On ObjPos[] Values (X, Y, Z)
	glGetFloatv(GL_MODELVIEW_MATRIX,Minv.Get());				// Retrieve ModelView Matrix From Minv
	wlp[0] = 0.0f;										// World Local Coord X To 0
	wlp[1] = 0.0f;										// World Local Coord Y To 0
	wlp[2] = 0.0f;										// World Local Coord Z To 0
	//wlp[3] = 1.0f;
  //Old Math3
	//VMatMult(Minv, wlp);								// We Store The Position Of The World Origin Relative To The
  //New AsmMath4
  wlp *= Minv;  // Local Coord. System In 'wlp' Array

  lp += wlp;
	//lp[0] += wlp[0];									// Adding These Two Gives Us The
	//lp[1] += wlp[1];									// Position Of The Light Relative To
	//lp[2] += wlp[2];									// The Local Coordinate System

	glPopMatrix();
	//End of little math...

}


void CModel::SetNumVertex(int num)
{
	m_numVertices = num;
}

void CModel::SetNumTriangles(int num)
{
	m_numTriangles = num;
}






void CModel::SetJointKeyframe(int jointIndex, int keyframeIndex, float time, float *parameter, bool isRotation)
{
//	assert( m_numJoints > jointIndex );

	Keyframe& keyframe = isRotation ? m_pJoints[jointIndex].m_pRotationKeyframes[keyframeIndex] :
		m_pJoints[jointIndex].m_pTranslationKeyframes[keyframeIndex];

	keyframe.m_jointIndex = jointIndex;
	keyframe.m_time = time;
	memcpy( (void*)&keyframe.m_parameter, parameter, sizeof( float )*3 );

}

void CModel::SetupJoints()
{
	int i;
	for ( i = 0; i < m_numJoints; i++ )
	{
		Joint& joint = m_pJoints[i];

    joint.m_relative.SetRotationRadians(joint.m_localRotation);
		joint.m_relative.SetTranslation( joint.m_localTranslation);

		if ( joint.m_parent != -1 )
		{
			joint.m_absolute.Set( m_pJoints[joint.m_parent].m_absolute.Get());
			//old Math3
      //joint.m_absolute.postMultiply( joint.m_relative );
      //New AsmMath4
      joint.m_absolute *= joint.m_relative;
		}
		else
			joint.m_absolute.Set( joint.m_relative.Get());
	}

	for ( i = 0; i < m_numVertices; i++ )
	{
		Vertex& vertex = m_pVertices[i];

		if ( vertex.m_boneID != -1 )
		{
			//const Matrix4& matrix = m_pJoints[vertex.m_boneID].m_absolute;
			Vector3 tmpvec;
			tmpvec.x = vertex.m_location.x;
			tmpvec.y = vertex.m_location.y;
			tmpvec.z = vertex.m_location.z;

			m_pJoints[vertex.m_boneID].m_absolute.GetInverseTranslateVec( &tmpvec );
			m_pJoints[vertex.m_boneID].m_absolute.GetInverseRotateVec( &tmpvec );

			vertex.m_location = tmpvec;
			//vertex.m_location.y = tmpvec[1];
			//vertex.m_location.z = tmpvec[2];

		}
	}

	for ( i = 0; i < m_numTriangles; i++ ) {
		Triangle& triangle = m_pTriangles[i];
		for ( int j = 0; j < 3; j++ ) {
			const Vertex& vertex = m_pVertices[triangle.m_vertexIndices[j]];
			if ( vertex.m_boneID != -1 ) {
				//const Matrix4& matrix = m_pJoints[vertex.m_boneID].m_absolute;
				Vector3 tmpvec;
				tmpvec = triangle.m_vertexNormals[j];
				//tmpvec[1] = triangle.m_vertexNormals[j].y;
				//tmpvec[2] = triangle.m_vertexNormals[j].z;

				m_pJoints[vertex.m_boneID].m_absolute.GetInverseRotateVec( &tmpvec );

				triangle.m_vertexNormals[j] = tmpvec;
				//triangle.m_vertexNormals[j].y = tmpvec[1];
				//triangle.m_vertexNormals[j].z = tmpvec[2];

			}
		}
	}
}

void CModel::advanceAnimation()
{
	double time = m_pTimer->getTime();

	if ( time > m_totalTime )
	{
		if ( m_looping )
		{
			RestartAnimation();
			time = 0;
		}
		else
			time = m_totalTime;
	} 

	for ( int i = 0; i < m_numJoints; i++ )
	{
		Vector3 transVec;
		Matrix4 transform;
		int frame;
		Joint *pJoint = &m_pJoints[i];

		if ( pJoint->m_numRotationKeyframes == 0 && pJoint->m_numTranslationKeyframes == 0 )
		{
			pJoint->m_final.Set( pJoint->m_absolute.Get());
			continue;
		}

		frame = pJoint->m_currentTranslationKeyframe;
		while ( frame < pJoint->m_numTranslationKeyframes && pJoint->m_pTranslationKeyframes[frame].m_time < time )
		{
			frame++;
		}
		pJoint->m_currentTranslationKeyframe = frame;

		if ( frame == 0 )
			transVec = pJoint->m_pTranslationKeyframes[0].m_parameter;
		else if ( frame == pJoint->m_numTranslationKeyframes )
			transVec = pJoint->m_pTranslationKeyframes[frame-1].m_parameter;
		else
		{
			//assert( frame > 0 && frame < pJoint->m_numTranslationKeyframes );

			const CModel::Keyframe& curFrame = pJoint->m_pTranslationKeyframes[frame];
			const CModel::Keyframe& prevFrame = pJoint->m_pTranslationKeyframes[frame-1];

			float timeDelta = curFrame.m_time-prevFrame.m_time;
			float interpValue = ( float )(( time-prevFrame.m_time )/timeDelta );

			transVec = prevFrame.m_parameter + (curFrame.m_parameter - prevFrame.m_parameter)*interpValue;
      //transVec.x = prevFrame.m_parameter.x+( curFrame.m_parameter.x-prevFrame.m_parameter.x )*interpValue;
			//transVec.y = prevFrame.m_parameter.y+( curFrame.m_parameter.y-prevFrame.m_parameter.y )*interpValue;
			//transVec.z = prevFrame.m_parameter.z+( curFrame.m_parameter.z-prevFrame.m_parameter.z )*interpValue; 
		}

		frame = pJoint->m_currentRotationKeyframe;
		while ( frame < pJoint->m_numRotationKeyframes && pJoint->m_pRotationKeyframes[frame].m_time < time )
		{
			frame++;
		}
		pJoint->m_currentRotationKeyframe = frame;

		if ( frame == 0 )
			transform.SetRotationRadians( pJoint->m_pRotationKeyframes[0].m_parameter );
		else if ( frame == pJoint->m_numRotationKeyframes )
			transform.SetRotationRadians( pJoint->m_pRotationKeyframes[frame-1].m_parameter );
		else
		{
			//assert( frame > 0 && frame < pJoint->m_numRotationKeyframes );

			const CModel::Keyframe& curFrame = pJoint->m_pRotationKeyframes[frame];
			const CModel::Keyframe& prevFrame = pJoint->m_pRotationKeyframes[frame-1];
			
			float timeDelta = curFrame.m_time-prevFrame.m_time;
			float interpValue = ( float )(( time-prevFrame.m_time )/timeDelta );

			//assert( interpValue >= 0 && interpValue <= 1 );

#if 0
			Quaternion qPrev( prevFrame.m_parameter );
			Quaternion qCur( curFrame.m_parameter );
			Quaternion qFinal( qPrev, qCur, interpValue );
			transform.setRotationQuaternion( qFinal );
#else
			Vector3 rotVec;

			rotVec = prevFrame.m_parameter+( curFrame.m_parameter-prevFrame.m_parameter )*interpValue;
			//rotVec[1] = prevFrame.m_parameter[1]+( curFrame.m_parameter[1]-prevFrame.m_parameter[1] )*interpValue;
			//rotVec[2] = prevFrame.m_parameter[2]+( curFrame.m_parameter[2]-prevFrame.m_parameter[2] )*interpValue;

			transform.SetRotationRadians( rotVec );
#endif
		}

		transform.SetTranslation( transVec );
		Matrix4 relativeFinal( pJoint->m_relative );
		relativeFinal *= transform;

		if ( pJoint->m_parent == -1 )
			pJoint->m_final.Set( relativeFinal.Get());
		else
		{
			pJoint->m_final.Set( m_pJoints[pJoint->m_parent].m_final.Get());
			pJoint->m_final *= relativeFinal;
		}
	}

}

void CModel::RestartAnimation()
{
	for ( int i = 0; i < m_numJoints; i++ )
	{
		m_pJoints[i].m_currentRotationKeyframe = 0;
    m_pJoints[i].m_currentTranslationKeyframe = 0;
		m_pJoints[i].m_final.Set( m_pJoints[i].m_absolute.Get());
	}

	m_pTimer->reset();
}


void CModel::GetTriangle(int ntri,Vector3 *v1, Vector3 *v2, Vector3 *v3)
{
	*v1 = m_pVertices[m_pTriangles[ntri].m_vertexIndices[0]].m_location;
	*v2 = m_pVertices[m_pTriangles[ntri].m_vertexIndices[1]].m_location;
	*v3 = m_pVertices[m_pTriangles[ntri].m_vertexIndices[2]].m_location;
}

int CModel::GetNumTris()
{
	return m_numTriangles;
}

void CModel::StartAnimation()
{
	m_bAnimated = true;
	RestartAnimation();
}

void CModel::StopAnimation()
{
	m_bAnimated = false;
	advanceAnimation();
}

int CModel::GetActualFrame()
{
		Joint *pJoint = &m_pJoints[0];
		if (pJoint)
			return pJoint->m_currentTranslationKeyframe;
		else
			return 0;
}

void CModel::SetLooping(bool value)
{
	m_looping = value;
}

void CModel::PrepareArrays()
{
	if (m_bAnimated)
  {
		advanceAnimation();
  }

  int vertp = 0;
  int normp = 0;
  int idx = 0;
  m_numVerticesArray = 0;

	// Draw by group
	for ( int i = 0; i < m_numMeshes; i++ )
	{

		for ( int j = 0; j < m_pMeshes[i].m_numTriangles; j++ )
		{
			int triangleIndex = m_pMeshes[i].m_pTriangleIndices[j];
			const Triangle* pTri = &m_pTriangles[triangleIndex];

			for ( int k = 0; k < 3; k++ )
			{
				int index = pTri->m_vertexIndices[k];

				// { NEW }
				if ( m_pVertices[index].m_boneID == -1 )
				{
					//glVertex3f( m_pVertices[index].m_location.x,m_pVertices[index].m_location.y,m_pVertices[index].m_location.z );
          m_pv3VertArray[index] = m_pVertices[index].m_location;
          //vertp++;
				}
				else
				{
					// rotate according to transformation matrix
					const Matrix4& final = m_pJoints[m_pVertices[index].m_boneID].m_final;

					Vector3 newVertex(m_pVertices[index].m_location.x, m_pVertices[index].m_location.y, m_pVertices[index].m_location.z);
					newVertex.Transform( (float*)&final );
					//glVertex3f(newVertex.x,newVertex.y,newVertex.z);
          m_pv3VertArray[index] = newVertex;
          //vertp++;

				}

        m_piIndexes[idx] = index;
        idx++;

				// { end NEW }
			}
		}
	}

  m_numVerticesArray = idx;  
}
