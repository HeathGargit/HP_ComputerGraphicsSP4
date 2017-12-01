#ifndef GAMEOBJECT
#define GAMEOBJECT

#include "Objectinator.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class GameObject
{
public:
	GameObject();
	~GameObject();

	void update();
	void draw(unsigned int ProgramId);

protected:
	Objectinator* m_Object;
	glm::vec3 m_Position;
};

#endif