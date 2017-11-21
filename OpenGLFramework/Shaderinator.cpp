#include "Shaderinator.h"
#include <fstream>
#include <string>
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>



Shaderinator::Shaderinator(char* inputString, unsigned int shaderType)
{
	LoadShader(inputString, shaderType);
}


Shaderinator::~Shaderinator()
{
	glDeleteShader(m_Shader);
}

unsigned int Shaderinator::getShader()
{
	return m_Shader;
}

bool Shaderinator::LoadShader(char* inputString, unsigned int shaderType)
{
	//assume it's a file
	std::ifstream in;
	in.open(inputString);

	//if the file opened
	if (in.is_open())
	{
		//load into a string because this was the first way i found to do it on google
		std::string fileContents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

		//load the shader into the raw buffer
		m_RawShader = fileContents.c_str();

		//compile the shader
		m_Shader = glCreateShader(shaderType);

		glShaderSource(m_Shader, 1, &m_RawShader, 0);
		glCompileShader(m_Shader);
	}
	else
	{
		m_RawShader = inputString;

		glShaderSource(m_Shader, 1, &m_RawShader, 0);
		m_Shader = glCreateShader(shaderType);
		glCompileShader(m_Shader);
	}
	return true;
}