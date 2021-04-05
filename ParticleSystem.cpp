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
#include <cassert>

void CParticle::Create(CParticleSystem *parent, float time_counter) {
    //This particle is dead, so its free to mess around with.
	//Now, this is where the fun starts.  Kick butt baby.
	m_fAge = 0.0f;
	m_fDying_age = parent->m_fLife + rand() / RAND_MAX * parent->m_fLife_counter;

	//Now, same routine as above, except with size
	m_fSize = std::clamp(parent->m_fStart_size + RANDOM_FLOAT * parent->m_fSize_counter, MIN_SIZE, MAX_SIZE);
	m_fSize_counter = (parent->m_fEnd_size - m_fSize) / m_fDying_age;

	//Now, we calculate the velocity that the particle would 
	//have to move to from prev_location to current_location
	//in time_counter seconds.
    const Vector3 temp_velocity = (parent->GetPosition() - parent->m_v3Prev_location) / time_counter;

	//Now emit the particle from a location between the last
	//known location, and the current location.
	m_v3Position.x = parent->m_v3Prev_location.x + temp_velocity.x * RANDOM_FLOAT * time_counter;
	m_v3Position.y = parent->m_v3Prev_location.y + temp_velocity.y * RANDOM_FLOAT * time_counter;
	m_v3Position.z = parent->m_v3Prev_location.z + temp_velocity.z * RANDOM_FLOAT * time_counter;

	//Now a simple randomization of the point that the particle
	//is emitted from.
	m_v3Position.x += Random(parent->m_fSpread_min, parent->m_fSpread_max) / parent->m_fSpread_factor;
	m_v3Position.y += Random(parent->m_fSpread_min, parent->m_fSpread_max) / parent->m_fSpread_factor;
	m_v3Position.z += Random(parent->m_fSpread_min, parent->m_fSpread_max) / parent->m_fSpread_factor;

	//Update the previous location so the next update can 
	//remember where we were
	parent->m_v3Prev_location = parent->GetPosition();

	//The emitter has a direction.  This is where we find it:
    const float random_yaw = RANDOM_FLOAT * PI * 2.0f;
    const float random_pitch = DEG_TO_RAD(RANDOM_FLOAT * parent->m_fAngle);

	//The following code uses spherical coordinates to randomize
	//the velocity vector of the particle
	m_v3Velocity.x = cosf(random_pitch) * parent->m_v3Velocity.x;
	m_v3Velocity.y = sinf(random_pitch) * cosf(random_yaw) * parent->m_v3Velocity.y;
	m_v3Velocity.z = sinf(random_pitch) * sinf(random_yaw) * parent->m_v3Velocity.z;

	//Velocity at this point is just a direction (normalized
	//vector) and needs to be multiplied by the speed 
	//component to be legit.
	m_v3Velocity *= std::clamp(parent->m_fSpeed + RANDOM_FLOAT * parent->m_fSpeed_counter, MIN_SPEED, MAX_SPEED);

	SetParentSystem(parent);

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

	//Move the particle's current location
	m_v3Position += m_v3Velocity*time_counter;

	//Update the particle's velocity by the gravity vector by time.
	m_v3Velocity += m_pParent->gravity * time_counter;

	//Adjust current size 
	m_fSize += m_fSize_counter*time_counter;

	return true;
}



CParticleSystem::CParticleSystem() {
    m_uiParticles_per_sec = 0;
    m_uiParticles_numb_alive = 0;
    m_fStart_size = 0.0f;
    m_fSize_counter = 0.0f;
    m_fEnd_size = 0.0f;
    m_fSpeed = 0.0f;
    m_fSpeed_counter = 0.0f;
    m_fLife = 0.0f;
    m_fLife_counter = 0.0f;
    m_fAngle = 0.0f;
    m_fAge = 0.0f;
    m_fLast_update = 0.0f;
    m_fSpread_min = 0.0f;
    m_fSpread_max = 0.0f;
    m_fSpread_factor = 1.0f;
}

CParticleSystem::~CParticleSystem()
{

}

//This sets the emitter aperture half angle
//10 = a thin directional emitter
//45 = a directional emitter
//90 = emits in half a sphere
//180 = an omnidirectional emitter
void CParticleSystem::SetAngle(float half_angle) {
    m_fAngle = half_angle;
}

//sets the gravity direction
void CParticleSystem::SetGravity(float x, float y, float z) {
    gravity.x = x;
    gravity.y = y;
    gravity.z = z;
}

//set particles life in seconds
void CParticleSystem::SetLife(float seconds) {
  m_fLife = seconds;	
}

//set the emitter's generation speed in partiles per second
void CParticleSystem::SetParticlesPerSec(unsigned int number) {
  m_uiParticles_per_sec = number;	
}

void CParticleSystem::SetSize(float start, float end) {
	m_fStart_size=start;
	m_fEnd_size=end;
}

void CParticleSystem::SetSpeed(float value) {
  m_fSpeed = value;
}

//- Descriptions: Set the particle's spread (random spread around
//-				  the emitter's position).  You can set the min
//-               max, and the answer of that is divided by the
//-               spread_factor.
void CParticleSystem::SetSpread(float min, float max, float value) {
	m_fSpread_min = min;
	m_fSpread_max = max;

	if(value == 0)
		value = 1;

	m_fSpread_factor = value;

}

void CParticleSystem::SetVelocity(Vector3 vel) {
  m_v3Velocity = vel;
}

void CParticleSystem::Draw(float time) {
	glPushMatrix();
	glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_PIXEL_MODE_BIT);
	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_TEXTURE_2D);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	for (int loop = 0; loop < MAX_PARTICLES; loop++) {
		float size = particle[loop].m_fSize / 2;

		if (particle[loop].m_fAge >= DEATH_AGE) {
			glBegin(GL_TRIANGLE_STRIP);
			glVertex3f(
				particle[loop].GetPosition().x - size,
				particle[loop].GetPosition().y + size,
				particle[loop].GetPosition().z);

			glVertex3f(
				particle[loop].GetPosition().x + size,
				particle[loop].GetPosition().y + size,
				particle[loop].GetPosition().z);

			glVertex3f(
				particle[loop].GetPosition().x - size,
				particle[loop].GetPosition().y - size,
				particle[loop].GetPosition().z);

			glVertex3f(
				particle[loop].GetPosition().x + size,
				particle[loop].GetPosition().y - size,
				particle[loop].GetPosition().z);

			glEnd();
		}
	}
	glPopAttrib();
	glPopMatrix();
}

bool CParticleSystem::StepOver(float time, float num_to_create)
{
	int loop;
	unsigned int particles_created;
	float time_counter;
	float particles_needed=num_to_create;

	//We need the calculate the elapsed time
	time_counter = (time - m_fLast_update);

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

	//Now calculate how many particles we should create based on 
	//time and taking the previous frame's emission residue into
	//account.  This is where things start to get kewl.  
	//Of course, when you're dealing with particles, everything is kewl.
	particles_needed += (m_uiParticles_per_sec * time_counter);
	
	//Now, taking the previous line into account, we now cast 
	//particles_needed into an unsigned int, so that we aren't
	//going to try to create half of a particle or something.
	particles_created = (unsigned int) particles_needed;

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

	return true;

}
