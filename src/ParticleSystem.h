// ParticleSystem.h: interface for the CParticleSystem class.
//
//////////////////////////////////////////////////////////////////////

#ifndef PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

#include "asmmath2.h"
#include <algorithm>

constexpr int MAX_PARTICLES = 4000;

constexpr float DEATH_AGE = 0.f;

constexpr float MIN_SPEED = 0.f;
constexpr float MIN_SIZE = 0.1f;

constexpr float MAX_SPEED = 300.f;
constexpr float MAX_SIZE = 100.f;


class CParticleSystem;

class CParticle {
public:
    bool UpdatePart(float time_counter);
    void SetParentSystem(CParticleSystem* psys) { this->m_pParent = psys; }
    void Create(CParticleSystem* parent, float time_counter);

    const Vector3& GetPosition() const { return m_v3Position; }
    float GetSize() const { return m_fSize; }
    float GetAge() const { return m_fAge; }

    Vector3 m_v3Velocity{};		//The particle's current velocity

    float m_fSize{};			//The particle's current size
    float m_fSize_counter{};	//Adds/subtracts transparency over time

    float m_fAge{};				//The particle's current age
    float m_fDying_age{};		//The age at which the particle DIES!

    Vector3   m_v3Position{};
private:
    CParticleSystem* m_pParent{ nullptr };
};

class CParticleSystem {
public:
    void SetPosition(const Vector3& v) {
        m_v3Position = v;
    }

    const Vector3& GetPosition() const {
        return m_v3Position;
    }

    void StepOver(float time, float num_to_create);
    void Draw();

    void SetVelocity(const Vector3& vel) {
        m_v3Velocity = vel;
    }

    void SetSpeed(float value) {
        m_fSpeed = value;
    }

    void SetSize(float start, float end) {
        m_fStart_size = start;
        m_fEnd_size = end;
    }

    //set the emitter's generation speed in partiles per second
    void SetParticlesPerSec(unsigned int number) {
        m_uiParticles_per_sec = number;
    }

    //set particles life in seconds
    void SetLife(float seconds) {
        m_fLife = seconds;
    }

    //sets the gravity direction
    void SetGravity(const Vector3& g) {
        gravity = g;
    }

    //This sets the emitter aperture half angle
    //10 = a thin directional emitter
    //45 = a directional emitter
    //90 = emits in half a sphere
    //180 = an omnidirectional emitter
    void SetAngle(float half_angle) {
        m_fAngle = half_angle;
    }

    Vector3 m_v3Prev_location;		//The last known location of the system
    Vector3 m_v3Velocity;			//The current known velocity of the system

    float m_fStart_size{};			//The starting size of the particles
    float m_fSize_counter{};		//Adds/subtracts particle size over time

    float m_fEnd_size{};			//The particle's end size (used for a MAX boundry)

    float m_fSpeed{};				//The system's speed

    float m_fLife{};				//The system's life (in seconds)

    float m_fAngle{};				//System's angle (90==1/2 sphere, 180==full sphere)

    Vector3 gravity{};		    	//Gravity for the X, Y, and Z axis
private:
    CParticle particle[MAX_PARTICLES];//All of our particles

    Vector3 m_v3Location{};			//The current known position of the system

    Vector3   m_v3Position{};
    unsigned int m_uiParticles_per_sec{};	//Particles emitted per second

    float m_fLast_update{};			//The last time the system was updated
};

#endif // PARTICLESYSTEM_H_
