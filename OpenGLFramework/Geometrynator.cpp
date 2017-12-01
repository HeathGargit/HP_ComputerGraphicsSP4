#include "Geometrynator.h"
#include "tiny_obj_loader.h"
#include "OBJVertex.h"
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


Geometrynator::Geometrynator(char * inputloc)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;

	//use stbi to read the image file into an unsigned int
	unsigned char* data = stbi_load(inputloc, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	//check image format
	GLint format = 0;
	switch (imageFormat) {
	case STBI_grey:
		format = GL_RED;
		break;
	case STBI_grey_alpha:
		format = GL_RG;
		break;
	case STBI_rgb:
		format = GL_RGB;
		break;
	case STBI_rgb_alpha:
		format = GL_RGBA;
		break;
	default:
		break;
	};

	//pass the loaded image to the graphics card through a handle
	glGenTextures(1, &m_diffuse_material); //the handle
	glBindTexture(GL_TEXTURE_2D, m_diffuse_material);
	glTexImage2D(GL_TEXTURE_2D, 0, format, imageWidth, imageHeight, 0, format, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//unload the image
	stbi_image_free(data);

	//generate the surface to put the texture on.
	generateGround();

	//generateGrid(m_rows, m_cols);
}


Geometrynator::~Geometrynator()
{
}

void Geometrynator::GenerateGround()
{
	float vertexData[] = {
		-5, 0, 5, 1, 0, 1, 5, 0, 5, 1, 1, 1, 5, 0, -5, 1, 1, 0, -5, 0, -5, 1, 0, 0,
	};

	unsigned int indexData[] = {
		0, 1, 2, 0, 2, 3,
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, vertexData, GL_STATIC_DRAW);
	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0) + 16);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
