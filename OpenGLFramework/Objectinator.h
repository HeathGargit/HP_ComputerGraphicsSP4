#ifndef OBJECTINATOR
#define OBJECTINATOR

#include "OpenGLInfo.h"
#include "tiny_obj_loader.h"
#include <glm/vec3.hpp>

class Objectinator
{
public:
	Objectinator(char* inputString);
	Objectinator(char* fileInputString, char* materialLocationInputString);
	~Objectinator();

	void draw(const unsigned int programID);
	void setWorldPos(glm::vec3 newPos);

private:
	void createOpenGLBuffers(tinyobj::attrib_t& attribs, std::vector<tinyobj::shape_t>& shapes, std::vector<tinyobj::material_t>& materials, const char* mat_loc);

	std::vector<OpenGLInfo> m_glInfo;
	unsigned int m_diffuse_material;
	unsigned int m_specular_material;
	unsigned int m_normal_material;

	glm::vec3 m_WorldPosition;
};

#endif