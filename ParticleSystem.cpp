// ParticleSystem.cpp: implementation of the CParticleSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "ParticleSystem.h"

#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#endif /* WIN32 */
#include <gl/GL.h>

#include "AsmMath4.h"
#include "utils.h"
#include <algorithm>
#include <cassert>

void CParticle::Create(CParticleSystem* parent, float time_counter) {
    //This particle is dead, so its free to mess around with.
    //Now, this is where the fun starts.  Kick butt baby.
    m_fAge = 0.f;
    m_fDying_age = parent->m_fLife;

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
    const float random_yaw = RANDOM_FLOAT * PI * 2.f;
    const float random_pitch = DEG_TO_RAD(RANDOM_FLOAT * parent->m_fAngle);

    //The following code uses spherical coordinates to randomize
    //the velocity vector of the particle
    m_v3Velocity.x = cosf(random_pitch) * parent->m_v3Velocity.x;
    m_v3Velocity.y = sinf(random_pitch) * cosf(random_yaw) * parent->m_v3Velocity.y;
    m_v3Velocity.z = sinf(random_pitch) * sinf(random_yaw) * parent->m_v3Velocity.z;

    //Velocity at this point is just a direction (normalized
    //vector) and needs to be multiplied by the speed 
    //component to be legit.
    m_v3Velocity *= std::clamp(parent->m_fSpeed, MIN_SPEED, MAX_SPEED);

    SetParentSystem(parent);
}

bool CParticle::UpdatePart(float time_counter) {
    //Age the particle by the time counter
    m_fAge += time_counter;

    if (m_fAge >= m_fDying_age) {
        m_fAge = -1.f;
        return false;
    }

    //Move the particle's current location
    m_v3Position += m_v3Velocity * time_counter;

    //Update the particle's velocity by the gravity vector by time.
    m_v3Velocity += m_pParent->gravity * time_counter;

    //Adjust current size 
    m_fSize += m_fSize_counter * time_counter;

    return true;
}

void CParticleSystem::Draw() {
    glPushMatrix();
    glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_PIXEL_MODE_BIT);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);

    glColor4f(1.f, 1.f, 1.f, 1.f);
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

void CParticleSystem::StepOver(float time, float num_to_create) {
    //We need the calculate the elapsed time
    float time_counter = (time - m_fLast_update);

    //Set the time of the last update to now.  Its sort of wierd
    //doing all this stuff that is in the past, now... Its starting
    //to mess with my brain, oh darn, I guess I need another Mountain
    //Dew, well isn't that a shame. ^_^
    m_fLast_update = time;

    //Update all particles
    for (int loop = 0; loop < MAX_PARTICLES; loop++) {
        if (particle[loop].m_fAge >= DEATH_AGE) {
            particle[loop].UpdatePart(time_counter);
        }
    }

    //Now calculate how many particles we should create based on 
    //time and taking the previous frame's emission residue into
    //account.  This is where things start to get kewl.  
    //Of course, when you're dealing with particles, everything is kewl.
    //Now, taking the previous line into account, we now cast 
    //particles_needed into an unsigned int, so that we aren't
    //going to try to create half of a particle or something.
    unsigned int particles_needed = (unsigned int)(num_to_create + m_uiParticles_per_sec * time_counter);

    //Lets make sure that we actually have a particle, or two, or
    //thousand to create
    if (particles_needed < 1) {
        m_v3Prev_location = m_v3Location;
        return;
    }

    for (int loop = 0; loop < MAX_PARTICLES; loop++) {
        //If we have created enough particles to satisfy the needed
        //amount, then this value will be zero, and will quit this 
        //loop faster than Nixon resigned from office.
        if (!particles_needed)
            break;

        //If the age of this particles is -1.0, then this particle
        //is not in use.  The poor guy died probably.  Damn shame.
        if (particle[loop].m_fAge <= DEATH_AGE) {
            particle[loop].Create(this, time_counter);

            //Now we decrease the amount of particles we need to create
            //by one.  
            particles_needed--;
        }
    }
}
