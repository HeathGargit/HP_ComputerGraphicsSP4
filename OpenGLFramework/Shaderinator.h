/*---------------------------------------------------------
File Name: Shaderinator.h
Purpose: class that sets up shaders to be added to Programs.
Author: Heath Parkes (gargit@gargit.net)
Modified: 1/12/2017
-----------------------------------------------------------
Copyright 2017 AIE/HP
---------------------------------------------------------*/

#ifndef SHADERINATOR
#define SHADERINATOR

class Shaderinator
{
public:
	//constructors
	Shaderinator(char* inputString, unsigned int shaderType);
	~Shaderinator();
	
	//accessor
	unsigned int getShader();

private:
	//funcs
	bool LoadShader(char* inputString, unsigned int shaderType);

	//member variables
	const char* m_RawShader;
	unsigned int m_Shader;
};

#endif