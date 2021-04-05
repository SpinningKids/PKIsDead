// ParticleSystem.cpp: implementation of the CParticleSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "ParticleSystem.h"

#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#endif /* WIN32 */
#include <GL/gl.h>

#include "utils.h"
#include <algorithm>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CParticle::CParticle()
{
  m_fAge = 0.0f;
  m_fDying_age = 0.0f;
  m_fSize = 0.0f;
  m_fSize_counter = 0.0f;
	m_fAlpha = 0.0f;
  m_fAlpha_counter = 0.0f;

	color.r = 0.0f;		//Set all of the vectors to 0.0 
	color.g = 0.0f; 
	color.b = 0.0f; 
	color.a = 0.0f;

	color_counter.r = 0.0f;
	color_counter.g = 0.0f;
	color_counter.b = 0.0f;
	color_counter.a = 0.0f;
}

CParticle::~CParticle()
{

}

void CParticle::Create(CParticleSystem *parent, float time_counter)
{
	Vector3 temp_velocity;
	float random_yaw;
	float random_pitch;
	float new_speed;

	//This particle is dead, so its free to mess around with.
	//Now, this is where the fun starts.  Kick butt baby.
	m_fAge = 0.0f;
	m_fDying_age = (parent->m_fLife)+((rand() / RAND_MAX)*(parent->m_fLife_counter));

	//Now, we are going to set the particle's color.  The color
	//is going to be the system's start color*color counter
	color.r = (parent->start_color.r)+(rand() / RAND_MAX)*(parent->color_counter.r);
	color.g = (parent->start_color.g)+(rand() / RAND_MAX)*(parent->color_counter.g);
	color.b = (parent->start_color.b)+(rand() / RAND_MAX)*(parent->color_counter.b);
	color.a = 1.0f;

	//Now, lets calculate the color's counter, so that by the
	//time the particle is ready to die (poor guy), it will
	//have reached the system's end color.
	color_counter.r = ((parent->end_color.r)-color.r) / m_fDying_age;
	color_counter.g = ((parent->end_color.g)-color.g) / m_fDying_age;
	color_counter.b = ((parent->end_color.b)-color.b) / m_fDying_age;

	//Calculate the particle's alpha from the system's.
	this->m_fAlpha = (parent->m_fStart_alpha)+(RANDOM_FLOAT*(parent->m_fAlpha_counter));
	//Make sure the result of the above line is legal
	this->m_fAlpha = std::clamp(this->m_fAlpha, MIN_ALPHA, MAX_ALPHA);
	//Calculate the particle's alpha counter so that by the
	//time the particle is ready to die, it will have reached 
	//the system's end alpha
	m_fAlpha_counter=((parent->m_fEnd_alpha)-this->m_fAlpha) / m_fDying_age;

	//Now, same routine as above, except with size
	m_fSize = (parent->m_fStart_size)+(RANDOM_FLOAT*(parent->m_fSize_counter));
	m_fSize = std::clamp(m_fSize, MIN_SIZE, MAX_SIZE);
	m_fSize_counter = ((parent->m_fEnd_size) - m_fSize) / m_fDying_age;

	//Now, we calculate the velocity that the particle would 
	//have to move to from prev_location to current_location
	//in time_counter seconds.
	temp_velocity.x= ((parent->GetPosition().x)-(parent->m_v3Prev_location.x))/time_counter;
	temp_velocity.y= ((parent->GetPosition().y)-(parent->m_v3Prev_location.y))/time_counter;
	temp_velocity.z= ((parent->GetPosition().z)-(parent->m_v3Prev_location.z))/time_counter;

	//Now emit the particle from a location between the last
	//known location, and the current location.
	m_v3Position.x = (parent->m_v3Prev_location.x)+temp_velocity.x*RANDOM_FLOAT*time_counter;
	m_v3Position.y = (parent->m_v3Prev_location.y)+temp_velocity.y*RANDOM_FLOAT*time_counter;
	m_v3Position.z = (parent->m_v3Prev_location.z)+temp_velocity.z*RANDOM_FLOAT*time_counter;

	//Now a simple randomization of the point that the particle
	//is emitted from.
	m_v3Position.x+=(Random((parent->m_fSpread_min), (parent->m_fSpread_max)))/(parent->m_fSpread_factor);
	m_v3Position.y+=(Random((parent->m_fSpread_min), (parent->m_fSpread_max)))/(parent->m_fSpread_factor);
	m_v3Position.z+=(Random((parent->m_fSpread_min), (parent->m_fSpread_max)))/(parent->m_fSpread_factor);

	//Update the previous location so the next update can 
	//remember where we were
	(parent->m_v3Prev_location.x)=(parent->GetPosition().x);
	(parent->m_v3Prev_location.y)=(parent->GetPosition().y);
	(parent->m_v3Prev_location.z)=(parent->GetPosition().z);

	//The emitter has a direction.  This is where we find it:
	random_yaw  = (float)(RANDOM_FLOAT*PI*2.0f);
	random_pitch= (float)(DEG_TO_RAD(RANDOM_FLOAT*((parent->m_fAngle))));

	//The following code uses spherical coordinates to randomize
	//the velocity vector of the particle
	m_v3Velocity.x=(cosf(random_pitch))*(parent->m_v3Velocity.x);
	m_v3Velocity.y=(sinf(random_pitch)*cosf(random_yaw))*(parent->m_v3Velocity.y);
	m_v3Velocity.z=(sinf(random_pitch)*sinf(random_yaw))*(parent->m_v3Velocity.z);

	//Velocity at this point is just a direction (normalized
	//vector) and needs to be multiplied by the speed 
	//component to be legit.
	new_speed = ((parent->m_fSpeed)+(RANDOM_FLOAT*(parent->m_fSpeed_counter)));
	new_speed = std::clamp(new_speed, MIN_SPEED, MAX_SPEED);
	m_v3Velocity.x*= new_speed;
	m_v3Velocity.y*= new_speed;
	m_v3Velocity.z*= new_speed;

	SetParentSystem(parent);

}

void CParticle::SetParentSystem(CParticleSystem *psys)
{
  this->m_pParent = psys;
}

bool CParticle::UpdatePart(float time_counter)
{
	static Vector3 attract_location;
	static Vector3 attract_normal;

	//Age the particle by the time counter
	m_fAge+= time_counter;

	if(m_fAge >= m_fDying_age)
	{
		m_fAge=-1.0f;
		return false;
	}
	
	//Set the particle's previous location with the location that
	//will be the old one by the time we get through this function
	m_v3Prev_location.x = m_v3Position.x;
	m_v3Prev_location.y = m_v3Position.y;
	m_v3Prev_location.z = m_v3Position.z;

	//Move the particle's current location
	m_v3Position.x += m_v3Velocity.x*time_counter;
	m_v3Position.y += m_v3Velocity.y*time_counter;
	m_v3Position.z += m_v3Velocity.z*time_counter;

	//Update the particle's velocity by the gravity vector by time.
	m_v3Velocity.x+= (m_pParent->gravity.x * time_counter);
	m_v3Velocity.y+= (m_pParent->gravity.y * time_counter);
	m_v3Velocity.z+= (m_pParent->gravity.z * time_counter);

	if(m_pParent->IsAttracting())	
	{
		//Find out where our Parent is located so we can track it
		attract_location.x = m_pParent->GetPosition().x;
		attract_location.y = m_pParent->GetPosition().y;
		attract_location.z = m_pParent->GetPosition().z;

		//Calculate the vector between the particle and the attractor
		attract_normal.x = attract_location.x - m_v3Position.x; 
		attract_normal.y = attract_location.y - m_v3Position.y; 
		attract_normal.z = attract_location.z - m_v3Position.z; 

		//can turn off attraction for certain axes to create 
		//some kewl effects (such as a tornado!)
		
		glNormal3fv((float*)&attract_normal);

		//If you decide to use this simple method you really should use a variable multiplier
		//instead of a hardcoded value like 25.0f
		m_v3Velocity.x+= attract_normal.x*5.0f*time_counter;
		m_v3Velocity.y+= attract_normal.y*5.0f*time_counter;
		m_v3Velocity.z+= attract_normal.z*5.0f*time_counter;
	}

	color.r+= color_counter.r *time_counter;
	color.g+= color_counter.g *time_counter;
	color.b+= color_counter.b *time_counter;

	//Adjust the alpha values (for transparency)
	m_fAlpha+= m_fAlpha_counter*time_counter;

	//Adjust current size 
	m_fSize += m_fSize_counter*time_counter;

  //things are a lot quick if we can use OpenGL's vector color function.
	color.a = m_fAlpha;

	return true;
}



CParticleSystem::CParticleSystem()
{
  m_bAttracting = false;
  m_bStopped = false;
	m_uiParticles_per_sec = 0;
	m_uiParticles_numb_alive = 0;
  m_fStart_size = 0.0f;
	m_fSize_counter = 0.0f;
  m_fEnd_size = 0.0f;
	m_fStart_alpha = 0.0f;
  m_fAlpha_counter = 0.0f;
	m_fEnd_alpha = 0.0f;
  m_fSpeed = 0.0f;
  m_fSpeed_counter = 0.0f;
	m_fLife = 0.0f;
  m_fLife_counter = 0.0f;
  m_fAngle = 0.0f;
  m_fAge = 0.0f;
	m_fLast_update = 0.0f;
  m_fEmission_residue = 0.0f;
	m_fSpread_min = 0.0f;
  m_fSpread_max = 0.0f;
  m_fSpread_factor = 1.0f;
	m_fAttraction_percent = 0.0f;

  m_iUpdateFlag = UPDATE_AND_CREATE;
}

CParticleSystem::~CParticleSystem()
{

}

void CParticleSystem::SetStartEndAlpha(float startalpha, float endalpha)
{
 	m_fStart_alpha = startalpha;
	m_fEnd_alpha = endalpha; 
}

//This sets the emitter aperture half angle
//10 = a thin directional emitter
//45 = a directional emitter
//90 = emits in half a sphere
//180 = an omnidirectional emitter
void CParticleSystem::SetAngle(float half_angle)
{
  m_fAngle = half_angle;
}

//emitter's attraction strenght range 0 - 100
void CParticleSystem::SetAttraction(unsigned int Attraction_Percent)
{
	if(Attraction_Percent>100)
		Attraction_Percent=100;
		
	m_fAttraction_percent= (Attraction_Percent/100.0f);
}

void CParticleSystem::SetStartColor(rgb_a start)
{
  start_color = start;
}

void CParticleSystem::SetEndColor(rgb_a end)
{
  end_color = end;
}

//sets the gravity direction
void CParticleSystem::SetGravity(float x, float y, float z)
{
	gravity.x = x;
	gravity.y = y;
	gravity.z = z;
}

//set particles life in seconds
void CParticleSystem::SetLife(float seconds)
{
  m_fLife = seconds;	
}

//set the emitter's generation speed in partiles per second
void CParticleSystem::SetParticlesPerSec(unsigned int number)
{
  m_uiParticles_per_sec = number;	
}

void CParticleSystem::SetSize(float start, float end)
{
	m_fStart_size=start;
	m_fEnd_size=end;
}

void CParticleSystem::SetSpeed(float value)
{
  m_fSpeed = value;
}

//- Descriptions: Set the particle's spread (random spread around
//-				  the emitter's position).  You can set the min
//-               max, and the answer of that is divided by the
//-               spread_factor.
void CParticleSystem::SetSpread(float min, float max, float value)
{
	m_fSpread_min = min;
	m_fSpread_max = max;

	if(value == 0)
		value = 1;

	m_fSpread_factor = value;

}

//This function sets a flag of the particle system
//either ON or OFF.  First you pass a constant to
//the flag argument, here are your valid choice:
//-           -MOVEMENT: Moves the emitter around the
//-  			               screen
//-						-ATTRACTION: Attracts the particles back
//-									 towards the emitter.
//-						-STOP: Suspends the particles from moving
//-                            and builds the amount that is going
//-                            to be created up (Builds up the
//-							   the amount to be created).
//-
//-				   Then for the second flag, you need to pass
//-                either SK_OFF, or SK_ON, depending on if you
//-                want the selected state on or off.
void CParticleSystem::SetSystemFlag(int flag, bool state)
{
	switch(flag)
	{
	  case ATTRACTION:
		  m_bAttracting=state;
		  break;

	  case STOP:
		  m_bStopped=state;
		  break;
	}

}

void CParticleSystem::SetVelocity(Vector3 vel)
{
  m_v3Velocity = vel;
}

void CParticleSystem::Draw(float time)
{
	int loop;
	float size;
	Vector3 vert;

  glPushMatrix();
	glScalef(m_v3Scale.x,m_v3Scale.y,m_v3Scale.z);
	glRotatef(m_v3Rotation.x,1,0,0);
	glRotatef(m_v3Rotation.y,0,1,0);
	glRotatef(m_v3Rotation.z,0,0,1);
  
	glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_PIXEL_MODE_BIT);
	//glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
	glEnable(GL_BLEND);
  glDisable(GL_CULL_FACE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_TEXTURE_2D);
	if (this->m_iTexture1ID > 0)
    glBindTexture(GL_TEXTURE_2D, m_iTexture1ID);
  else
    glDisable(GL_TEXTURE_2D);

	for(loop=0; loop<MAX_PARTICLES; loop++)
		{
		size = particle[loop].m_fSize/2;
		particle[loop].color[4]=particle[loop].GetAlpha();

		if(particle[loop].m_fAge>=DEATH_AGE)
			{
			glColor4fv((float*)&(particle[loop].color));
			
			glBegin(GL_TRIANGLE_STRIP);

				vert.x = particle[loop].GetPosition().x - size;
				vert.y = particle[loop].GetPosition().y + size;
				vert.z = particle[loop].GetPosition().z;

			  glTexCoord2d(1,1); 
        glVertex3fv((float*)&vert);

				vert.x = particle[loop].GetPosition().x + size;
				vert.y = particle[loop].GetPosition().y + size;
				vert.z = particle[loop].GetPosition().z;

				glTexCoord2d(0,1); 
        glVertex3fv((float*)&vert);

				vert.x = particle[loop].GetPosition().x - size;
				vert.y = particle[loop].GetPosition().y - size;
				vert.z = particle[loop].GetPosition().z;

				glTexCoord2d(1,0); 
        glVertex3fv((float*)&vert);

				vert.x = particle[loop].GetPosition().x + size;
				vert.y = particle[loop].GetPosition().y - size;
				vert.z = particle[loop].GetPosition().z;
			  
				glTexCoord2d(0,0); 
        glVertex3fv((float*)&vert);
			
			glEnd();
			}
		}		
	glPopAttrib();
  glPopMatrix();
  glEnable(GL_CULL_FACE);
  glDepthMask(GL_TRUE);

}

void CParticleSystem::PostOBJMessage(char *msg, float p1, float p2, float p3, float p4)
{
/*  SKString str(msg);
  str.UpperCase();

  if (str == "SETANGLE")
  {
    this->SetAngle(p1);
  }
  else if (str == "SETATTRACTION")
  {
    this->SetAttraction((unsigned int)p1);
  }
  else if (str == "SETSTARTCOLOR")
  {
    this->SetStartColor(rgb_a(p1,p2,p3,0));
  }
  else if (str == "SETENDCOLOR")
  {
    this->SetEndColor(rgb_a(p1,p2,p3,0));
  }
  else if (str == "SETGRAVITY")
  {
    this->SetGravity(p1,p2,p3);
  }
  else if (str == "SETVELOCITY")
  {
    this->SetVelocity(Vector3(p1,p2,p3));
  }
  else if (str == "SETLIFE")
  {
    this->SetLife(p1);
  }
  else if (str == "SETPPERSEC")
  {
    this->SetParticlesPerSec((unsigned int)p1);
  }
  else if (str == "SETSIZE")
  {
    this->SetSize(p1,p2);
  }
  else if (str == "SETSPEED")
  {
    this->SetSpeed(p1);
  }
  else if (str == "SETSPREAD")
  {
    this->SetSpread((int)p1,(int)p2,p3);
  }
  else if (str == "SETSEALPHA")
  {
    this->SetStartEndAlpha(p1,p2);
  }
  else if (str == "SETSYSFLAG")
  {
    if(p2 == 0)
      this->SetSystemFlag((int)p1,false);
    else
      this->SetSystemFlag((int)p1,true);
  }
  else if (str == "STEPOVER")
  {
    this->StepOver(p1 / 1000,p2);
  }
  else if (str == "SETUPDATE")
  {
    this->SetUpdateFlag(ONLY_UPDATE);
  }
  else if (str == "SETUPDATECREATE")
  {
    this->SetUpdateFlag(UPDATE_AND_CREATE);
  }
  else if (str == "SETCREATE")
  {
    this->SetUpdateFlag(ONLY_CREATE);
  }
  */
}

bool CParticleSystem::StepOver(float time, float num_to_create)
{
	int loop;
	unsigned int particles_created;
	float time_counter;
	float particles_needed=num_to_create;

	//We need the calculate the elapsed time
	time_counter = (time - m_fLast_update);

	if(m_iUpdateFlag == ONLY_UPDATE || m_iUpdateFlag == UPDATE_AND_CREATE)
	{
		//Set the time of the last update to now.  Its sort of wierd
		//doing all this stuff that is in the past, now... Its starting
		//to mess with my brain, oh darn, I guess I need another Mountain
		//Dew, well isn't that a shame. ^_^
		m_fLast_update = time;

		//Clear the particle counter variable before counting.
		m_uiParticles_numb_alive = 0;

		//Update all particles
		for(loop=0; loop<MAX_PARTICLES; loop++)
		{
			if(particle[loop].m_fAge>=DEATH_AGE)
			{
				if(particle[loop].UpdatePart(time_counter))
					m_uiParticles_numb_alive++;
			}
		}
	}

	if(m_iUpdateFlag == ONLY_CREATE || m_iUpdateFlag == UPDATE_AND_CREATE)
	{
		//Now calculate how many particles we should create based on 
		//time and taking the previous frame's emission residue into
		//account.  This is where things start to get kewl.  
		//Of course, when you're dealing with particles, everything is kewl.
		particles_needed += (m_uiParticles_per_sec * time_counter) + m_fEmission_residue;
		
		//Now, taking the previous line into account, we now cast 
		//particles_needed into an unsigned int, so that we aren't
		//going to try to create half of a particle or something.
		particles_created = (unsigned int) particles_needed;

		if(!m_bStopped)
		{
			//This will remember the difference between how many we wanted
			//to create, and how many we actually did create.  Doing it
			//this way, we aren't going to lose any accuracy.
			m_fEmission_residue = particles_needed - particles_created;
		}
		else
    {
			m_fEmission_residue= particles_needed;
			particles_created=0;
		}

		//Lets make sure that we actually have a particle, or two, or
		//thousand to create
		if(particles_created<1)
		{
			m_v3Prev_location.x = m_v3Location.x;
			m_v3Prev_location.y = m_v3Location.y;
			m_v3Prev_location.z = m_v3Location.z;
			return true;
		}

		for(loop=0; loop<MAX_PARTICLES; loop++)
		{
			//If we have created enough particles to satisfy the needed
			//amount, then this value will be zero, and will quit this 
			//loop faster than Nixon resigned from office.
			if(!particles_created)
				break;

			//If the age of this particles is -1.0, then this particle
			//is not in use.  The poor guy died probably.  Damn shame.
			if(particle[loop].m_fAge<=DEATH_AGE)
			{
				particle[loop].Create(this, time_counter);

				//Now we decrease the amount of particles we need to create
				//by one.  
				particles_created--;
			}
		}
	}

	return true;

}

void CParticleSystem::SetUpdateFlag(int flag)
{
  m_iUpdateFlag = flag;
}
