/*---------------------------------------------------------
File Name: Particle.h
Purpose: A particle class to be used with the particle emitter class. Pretty much just stores information about each particle
Author: Heath Parkes (gargit@gargit.net)
Modified: 1/12/2017
-----------------------------------------------------------
Copyright 2017 AIE/HP
---------------------------------------------------------*/

#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct Particle
{
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec4 colour;
	float size;
	float lifetime;
	float lifespan;
};

struct ParticleVertex
{
	glm::vec4 position;
	glm::vec4 colour;
};