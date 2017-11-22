#ifndef OBJECTINATOR
#define OBJECTINATOR

#include "OpenGLInfo.h"
#include "tiny_obj_loader.h"

class Objectinator
{
public:
	Objectinator(char* inputString);
	~Objectinator();

	void draw();

private:
	void createOpenGLBuffers(tinyobj::attrib_t& attribs, std::vector<tinyobj::shape_t>& shapes);

	std::vector<OpenGLInfo> m_glInfo;
};

#endif