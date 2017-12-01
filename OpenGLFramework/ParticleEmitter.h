#ifndef PARTICLEEMITTER
#define PARTICLEEMITTER

#include "Particle.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>



class ParticleEmitter
{
public:
	ParticleEmitter();
	virtual ~ParticleEmitter();

	void initialise(unsigned int a_MaxParticles, unsigned int a_EmitRate, float a_LifeTimeMin, float a_LifeTimeMax, float a_VelocityMin, float a_VelocityMax, float a_StartSize, float a_EndSize, const glm::vec4& a_StartColour, const glm::vec4& a_EngColour);

	void emit(); //emit a new particle
	void update(float deltaTime, const glm::mat4& a_CameraTransform); //updates each particle and manages life/death of particles
	void draw(); //draws the particles!

	int m_EmitRateAccessor;
	glm::vec3 m_direction;

protected:
	Particle*			m_Particles; //array of particle information
	unsigned int		m_FirstDead; //location of the first dead particle to loop up to
	unsigned int		m_MaxParticles; //maximum number of particles that can be emittered.

	unsigned int		m_VAO, m_VBO, m_IBO; //gl buffer thingos
	ParticleVertex*		m_VertexData; //array of data to draw

	glm::vec3			m_Position; //point where the particles emit from

	float				m_EmitTimer; //time between each emit?
	float				m_EmitRate; //something?

	float				m_LifespanMin; //length of time each particle will last at least
	float				m_LifespanMax; //max length of time particle will live

	float				m_VelocityMin; //speed of particle
	float				m_VelocityMax;

	float				m_StartSize; //size of particle
	float				m_EndSize;

	glm::vec4			m_StartColour; //colour of particle
	glm::vec4			m_EndColour;

	void setEmitRate(int newValue);
};

#endif