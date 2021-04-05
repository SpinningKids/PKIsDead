// ParticleSystem.h: interface for the CParticleSystem class.
//
//////////////////////////////////////////////////////////////////////

#ifndef PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

#include "Object.h"

#define MAX_PARTICLES 4000

#define ONLY_CREATE		  0
#define ONLY_UPDATE		  1
#define UPDATE_AND_CREATE 2

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
    float GetAlpha() const { return m_fAlpha; }
    float GetSize() const { return m_fSize; }
    const rgb_a& GetColor() const { return color; }
    float GetAge() const { return m_fAge; }

    Vector3 m_v3Velocity{ 0.f, 0.f, 0.f };		//The particle's current velocity

    rgb_a color{ 0.f, 0.f, 0.f, 0.f };			//The particle's color
    rgb_a color_counter{ 0.f, 0.f, 0.f, 0.f };	//The color counter!

    float m_fAlpha{ 0.f };
    float m_fAlpha_counter{ 0.f };	//Adds/subtracts transparency over time

    float m_fSize{ 0.f };				//The particle's current size
    float m_fSize_counter{ 0.f };		//Adds/subtracts transparency over time

    float m_fAge{ 0.f };				//The particle's current age
    float m_fDying_age{ 0.f };		//The age at which the particle DIES!

    Vector3   m_v3Position{ 0.f, 0.f, 0.f };
private:
    CParticleSystem* m_pParent{ nullptr };
};

class CParticleSystem : public CObject
{
public:
    void SetUpdateFlag(int flag);
    bool StepOver(float time, float num_to_create);
    void PostOBJMessage(char* msg, float p1, float p2, float p3, float p4);
    void Draw(float time);
    void SetVelocity(Vector3 vel);
    void SetSystemFlag(int flag, bool state);
    void SetSpread(float min, float max, float value);
    void SetSpeed(float value);
    void SetSize(float start, float end);
    void SetParticlesPerSec(unsigned int number);
    void SetLife(float seconds);
    void SetGravity(float x, float y, float z);
    void SetStartColor(rgb_a start);
    void SetEndColor(rgb_a end);
    void SetAttraction(unsigned int Attraction_Percent);
    void SetAngle(float half_angle);
    void SetStartEndAlpha(float startalpha, float endalpha);
    CParticleSystem();
    virtual ~CParticleSystem();

    bool IsAttracting(void) { return m_bAttracting; }
    bool IsStopped(void) { return m_bStopped; }

    CParticle particle[MAX_PARTICLES];//All of our particles

    Vector3 m_v3Prev_location;		//The last known location of the system
    Vector3 m_v3Location;			//The current known position of the system
    Vector3 m_v3Velocity;			//The current known velocity of the system

    float m_fStart_size;			//The starting size of the particles
    float m_fSize_counter;			//Adds/subtracts particle size over time
    float m_fEnd_size;				//The particle's end size (used for a MAX boundry)

    float m_fStart_alpha;			//The starting transparency of the particle
    float m_fAlpha_counter;		//Adds/subtracts particle's transparency over time
    float m_fEnd_alpha;			//The end transparency (used for a MAX boundry)

    rgb_a start_color;		//The starting color
    rgb_a color_counter;		//The color that we interpolate over time
    rgb_a end_color;			//The ending color

    float m_fSpeed;				//The system's speed
    float m_fSpeed_counter;		//The system's speed counter

    float m_fLife;					//The system's life (in seconds)
    float m_fLife_counter;			//The system's life counter

    float m_fAngle;				//System's angle (90==1/2 sphere, 180==full sphere)

    float m_fSpread_min;				//Used for random positioning around the emitter
    float m_fSpread_max;
    float m_fSpread_factor;		//Used to divide spread

    Vector3 gravity;			//Gravity for the X, Y, and Z axis
    float m_fAttraction_percent;

private:
    bool m_bAttracting;			//Is the system attracting particle towards itself?
    bool m_bStopped;				//Have the particles stopped emitting?

    unsigned int m_uiParticles_per_sec;	//Particles emitted per second
    unsigned int m_uiParticles_numb_alive;//The number of particles currently alive

    float m_fAge;					//The system's current age (in seconds)

    float m_fLast_update;			//The last time the system was updated

    float m_fEmission_residue;		//Helps emit very precise amounts of particles

    int m_iUpdateFlag;
};

#endif // PARTICLESYSTEM_H_
