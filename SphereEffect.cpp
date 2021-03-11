// SphereEffect.cpp


////////////////////////////////////////////////////////
//		includes

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#ifdef __WIN32__
#include <windows.h>
#endif /* __WIN32__ */
#include <GL/gl.h>
#include "SphereEffect.h"
#include "AsmMath4.h"

////////////////////////////////////////////////////////
//		globals

static int			g_subd;
static int			g_numVertex = 0;
static Vector3		* g_pVertex = NULL;

static float	g_toRad	= (2.0f*3.141592653589f/360.0f);

static Vector3		g_pos;
static Vector3		g_rot;



////////////////////////////////////////////////////////
//

// subd = 40, radius = 2
int initSphereObject( int subd, float radius )
{
	int i = 0, j = 0, offset = 0;
	float rad, angle = 360.0f/subd;
	float inc_theta, inc_phi;
	float phi, theta;
		
	assert(subd);
	
	g_subd = subd;
	g_numVertex = g_subd*g_subd;
	//rIO
  //g_pVertex = (Vector3 *) malloc( sizeof(Vertex) * g_numVertex );	
  g_pVertex = (Vector3 *) new Vector3 [g_numVertex];
	
	if( !g_pVertex )
		return -1;
	
	inc_phi = 360.0f / g_subd;
	inc_theta = 180.0f / g_subd;
	
	// modify the sphere shape
	for( phi=0; phi<180; phi += inc_phi, i++ )
	{
		for( theta=0; theta<360; theta += inc_theta, j++ )
		{
			rad = ( radius + 0.5*sin( g_toRad*(i*angle+j*angle) )*cos( g_toRad*(3*(i+j)*angle) ) );
			
			g_pVertex[offset].x = rad*radius * sin( phi*g_toRad ) * cos( theta*g_toRad );
			g_pVertex[offset].y = rad*radius * sin( phi*g_toRad ) * sin( theta*g_toRad );
			g_pVertex[offset].z = rad*radius * cos( phi*g_toRad );
			
			++offset;
		}
	}
	
	g_pos.x = 0;
	g_pos.y = 0;
	g_pos.z = -15;


	return 0;
}
	

	
void dSphereEffect(rgb_a pcolor, rgb_a vcolor, bool base, bool lines)
{
	int offset = 0;
		
	assert( g_numVertex && g_subd && g_pVertex );
	
	glPushMatrix();
		
	// state
	//glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT | GL_HINT_BIT | GL_POINT_BIT | GL_LINE_BIT );
	

	glDisable( GL_TEXTURE_2D );
	glEnable( GL_POINT_SMOOTH );
	glEnable( GL_BLEND );
	glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE );
		
	if (lines == false)
    glPointSize( 3 );
	
	// rendering
  if (lines == false)
	  glBegin( GL_POINTS );
  else
	  glBegin( GL_LINE_STRIP );
	
  glColor4f(pcolor.r,pcolor.g,pcolor.b,pcolor.a);
	for( offset = 0; offset < g_numVertex; ++offset )
	{
		glVertex3f( g_pVertex[offset].x, 
			g_pVertex[offset].y, 
			g_pVertex[offset].z );
	}
	
	glEnd();

  if (base) {
	
	  offset = 0;
	  
	  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	  
	  int vertexPerSpan = g_numVertex / g_subd, x, y;
	  
	  for( y = 0; y < g_subd-1; y++ )
	  {
		  glBegin( GL_TRIANGLE_STRIP );
		  
      glColor4f(vcolor.r,vcolor.g,vcolor.b,vcolor.a);
		  for( x = 0; x < vertexPerSpan; x++ )
		  {
			  glVertex3f( g_pVertex[offset].x, 
				  g_pVertex[offset].y, 
				  g_pVertex[offset].z );
			  
			  glVertex3f( g_pVertex[offset].x, 
				  g_pVertex[offset+vertexPerSpan].y, 
				  g_pVertex[offset].z );
			  
			  ++offset;
		  }
		  
		  glEnd();	
	  }
  }
	//glPopAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT | GL_HINT_BIT | GL_POINT_BIT | GL_LINE_BIT );

  glPopMatrix();
}


void unloadSphereEffect( void )
{
	if( g_pVertex )
		delete [] g_pVertex;
	glDisable( GL_FOG );
}


