// ParticleSystem.h: interface for the CParticleSystem class.
//
//////////////////////////////////////////////////////////////////////

#ifndef PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

#include "AsmMath4.h"

#define MAX_PARTICLES 4000

#define DEATH_AGE  0.0f

#define ATTRACTION 0
#define STOP       1

#define GET_X 0
#define GET_Y 1
#define GET_Z 2

#define SK_OFF false
#define SK_ON  true

#define MIN_SPEED	 0.0f
#define MIN_SIZE     0.1f
#define MIN_LIFETIME 0.01f
#define MIN_SPREAD	 0.01f
#define MIN_EMISSION 1.0f
#define MIN_GRAVITY -10.0f
#define MIN_ALPHA	 0.0f
#define MIN_COLOR	 0.0f

#define MAX_SPEED	 300.0f
#define MAX_SIZE     100.0f
#define MAX_LIFETIME 20.0f
#define MAX_SPREAD   180.0f
#define MAX_EMISSION 1000
#define MAX_GRAVITY  10.0f
#define MAX_ALPHA	 1.0f
#define MAX_COLOR	 1.0f


class CParticleSystem;

class CParticle
{
public:
    bool UpdatePart(float time_counter);
    void SetParentSystem(CParticleSystem* psys) { this->m_pParent = psys; }
    void Create(CParticleSystem* parent, float time_counter);

    const Vector3& GetPosition() const { return m_v3Position; }
    float GetSize() const { return m_fSize; }
    float GetAge() const { return m_fAge; }

    Vector3 m_v3Velocity{ 0.f, 0.f, 0.f };		//The particle's current velocity

    float m_fSize{ 0.f };				//The particle's current size
    float m_fSize_counter{ 0.f };		//Adds/subtracts transparency over time

    float m_fAge{ 0.f };				//The particle's current age
    float m_fDying_age{ 0.f };		//The age at which the particle DIES!

    Vector3   m_v3Position{ 0.f, 0.f, 0.f };
private:
    CParticleSystem* m_pParent{ nullptr };
};

class CParticleSystem {
public:
    void SetPosition(const Vector3* v) {
        m_v3Position = *v;
    }

    void SetPosition(float x, float y, float z) {
        m_v3Position.x = x;
        m_v3Position.y = y;
        m_v3Position.z = z;
    }

    const Vector3& GetPosition() const {
        return m_v3Position;
    }

    bool StepOver(float time, float num_to_create);
    void Draw(float time);
    void SetVelocity(Vector3 vel);
    void SetSpread(float min, float max, float value);
    void SetSpeed(float value);
    void SetSize(float start, float end);
    void SetParticlesPerSec(unsigned int number);
    void SetLife(float seconds);
    void SetGravity(float x, float y, float z);
    void SetAngle(float half_angle);
    CParticleSystem();
    virtual ~CParticleSystem();

    CParticle particle[MAX_PARTICLES];//All of our particles

    Vector3 m_v3Prev_location;		//The last known location of the system
    Vector3 m_v3Location;			//The current known position of the system
    Vector3 m_v3Velocity;			//The current known velocity of the system

    float m_fStart_size;			//The starting size of the particles
    float m_fSize_counter;			//Adds/subtracts particle size over time
    float m_fEnd_size;				//The particle's end size (used for a MAX boundry)

    float m_fSpeed;				//The system's speed
    float m_fSpeed_counter;		//The system's speed counter

    float m_fLife;					//The system's life (in seconds)
    float m_fLife_counter;			//The system's life counter

    float m_fAngle;				//System's angle (90==1/2 sphere, 180==full sphere)

    float m_fSpread_min;				//Used for random positioning around the emitter
    float m_fSpread_max;
    float m_fSpread_factor;		//Used to divide spread

    Vector3 gravity;			//Gravity for the X, Y, and Z axis

private:
    Vector3   m_v3Position{ 0.0f, 0.0f, 0.0f };
    unsigned int m_uiParticles_per_sec;	//Particles emitted per second
    unsigned int m_uiParticles_numb_alive;//The number of particles currently alive

    float m_fAge;					//The system's current age (in seconds)

    float m_fLast_update;			//The last time the system was updated
};

#endif // PARTICLESYSTEM_H_
