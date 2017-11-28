#include "ParticleEmitter.h"
#include <gl_core_4_4.h>
#include <glm/geometric.hpp>


ParticleEmitter::ParticleEmitter() :
	m_Particles(nullptr),
	m_FirstDead(0),
	m_MaxParticles(0),
	m_Position(0, 0, 0),
	m_VAO(0), m_VBO(0), m_IBO(0),
	m_VertexData(nullptr)
{
}


ParticleEmitter::~ParticleEmitter()
{
	delete[] m_Particles;
	delete[] m_VertexData;

	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_IBO);
}

void ParticleEmitter::initialise(unsigned int a_MaxParticles, unsigned int a_EmitRate, float a_LifeTimeMin, float a_LifeTimeMax, float a_VelocityMin, float a_VelocityMax, float a_StartSize, float a_EndSize, const glm::vec4 & a_StartColour, const glm::vec4 & a_EngColour)
{
	//set up emit timers
	m_EmitTimer = 0;
	m_EmitRate = 1.0f / a_EmitRate;

	//set up all variables
	m_StartColour = a_StartColour;
	m_EndColour = a_EngColour;
	m_StartSize = a_StartSize;
	m_EndSize = a_EndSize;
	m_VelocityMax = a_VelocityMax;
	m_VelocityMin = a_VelocityMin;
	m_LifespanMin = a_LifeTimeMin;
	m_LifespanMax = a_LifeTimeMax;
	m_MaxParticles = a_MaxParticles;

	//create particle array
	m_Particles = new Particle[m_MaxParticles];
	m_FirstDead = 0;

	//create the array of vertices for the particles
	//4 vertices per particle for a quad
	//will be filled during update
	m_VertexData = new ParticleVertex[m_MaxParticles * 4];

	//create the index buffer data for the particles
	//6 indicies per quad of 2 triangles
	//fill it now as it never changes
	unsigned int* indexData = new unsigned int[m_MaxParticles * 6];
	for (unsigned int i = 0; i < m_MaxParticles; ++i)
	{
		indexData[i * 6 + 0] = i * 4 + 0;
		indexData[i * 6 + 1] = i * 4 + 1;
		indexData[i * 6 + 2] = i * 4 + 2;

		indexData[i * 6 + 3] = i * 4 + 0;
		indexData[i * 6 + 4] = i * 4 + 2;
		indexData[i * 6 + 5] = i * 4 + 3;
	}

	//create opengl buffers
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_IBO);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, m_MaxParticles * 4 * sizeof(ParticleVertex), m_VertexData, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_MaxParticles * 6 * sizeof(unsigned int), indexData, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //colour
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), 0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] indexData;
}

void ParticleEmitter::emit()
{
	//only emit if there is a dead particle to use
	if (m_FirstDead >= m_MaxParticles)
	{
		return;
	}

	//resurrect the first dead pixel
	Particle& particle = m_Particles[m_FirstDead++];

	//assign it's starting position ## CHANGE THIS FOR YOUR RAIN EMITTER HEATH! ##
	particle.position = m_Position;

	//randomise it's lifetime
	particle.lifetime = 0;
	particle.lifespan = (rand() / (float)RAND_MAX) * (m_LifespanMax - m_LifespanMin) + m_LifespanMin;

	//set starting size and colour
	particle.colour = m_StartColour;
	particle.size = m_StartSize;

	//this is where we set the direction it travels.  ## CHANGE THIS FOR YOUR RAIN EMITTER HEATH! ##
	/*float velocity = (rand() / (float)RAND_MAX) * (m_VelocityMax - m_VelocityMin) + m_VelocityMin;
	particle.velocity.x = (rand() / (float)RAND_MAX) * 2 - 1;
	particle.velocity.y = (rand() / (float)RAND_MAX) * 2 - 1;
	particle.velocity.z = (rand() / (float)RAND_MAX) * 2 - 1;
	particle.velocity = glm::normalize(particle.velocity) * velocity;*/

	particle.velocity = glm::vec3(0, -1.0f, 0);
}

void ParticleEmitter::update(float deltaTime, const glm::mat4 & a_CameraTransform)
{
	using glm::vec3;
	using glm::vec4;

	//spawn particles
	m_EmitTimer += deltaTime;
	while (m_EmitTimer > m_EmitRate)
	{
		emit();
		m_EmitTimer -= m_EmitRate;
	}

	unsigned int quad = 0;

	//update particles and turn live particles into billboard quads
	for (unsigned int i = 0; i < m_FirstDead; i++)
	{
		Particle* particle = &m_Particles[i];

		particle->lifetime += deltaTime;
		if (particle->lifetime >= particle->lifespan) //if it's past it's life time, end it
		{
			*particle = m_Particles[m_FirstDead - 1];
			m_FirstDead--;
		}
		else //else process it
		{
			//move the particle
			particle->position += particle->velocity * deltaTime;

			//resize
			particle->size = glm::mix(m_StartSize, m_EndSize, particle->lifetime / particle->lifespan);

			//recolour particle
			particle->colour = glm::mix(m_StartColour, m_EndColour, particle->lifetime / particle->lifespan);

			//make a quad of the correct size and colour
			float halfsize = particle->size * 0.5f;

			m_VertexData[quad * 4 + 0].position = glm::vec4(halfsize, halfsize * 2, 0, 1);
			m_VertexData[quad * 4 + 0].colour = particle->colour;
			m_VertexData[quad * 4 + 1].position = glm::vec4(-halfsize, halfsize* 2, 0, 1);
			m_VertexData[quad * 4 + 1].colour = particle->colour;
			m_VertexData[quad * 4 + 2].position = glm::vec4(-halfsize, -halfsize * 2, 0, 1);
			m_VertexData[quad * 4 + 2].colour = particle->colour;
			m_VertexData[quad * 4 + 3].position = glm::vec4(halfsize, -halfsize * 2, 0, 1);
			m_VertexData[quad * 4 + 3].colour = particle->colour;

			//create the billboard transform to always point the particle towards the camera
			glm::vec3 zAxis = glm::normalize(glm::vec3(a_CameraTransform[3]) - particle->position);
			glm::vec3 xAxis = glm::cross(glm::vec3(a_CameraTransform[1]), zAxis);
			glm::vec3 yAxis = glm::cross(zAxis, xAxis);
			glm::mat4 billboard(glm::vec4(xAxis, 0), glm::vec4(yAxis, 0), glm::vec4(zAxis, 0), glm::vec4(0, 0, 0, 1));

			m_VertexData[quad * 4 + 0].position = billboard * m_VertexData[quad * 4 + 0].position + glm::vec4(particle->position, 0);
			m_VertexData[quad * 4 + 1].position = billboard * m_VertexData[quad * 4 + 1].position + glm::vec4(particle->position, 0);
			m_VertexData[quad * 4 + 2].position = billboard * m_VertexData[quad * 4 + 2].position + glm::vec4(particle->position, 0);
			m_VertexData[quad * 4 + 3].position = billboard * m_VertexData[quad * 4 + 3].position + glm::vec4(particle->position, 0);

			++quad;
		}
	}
}

void ParticleEmitter::draw()
{
	//sync the particle vertex buffer based on how many LIVING particles there are
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_FirstDead * 4 * sizeof(ParticleVertex), m_VertexData);

	//draw 'em!
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_FirstDead * 6, GL_UNSIGNED_INT, 0);
}
