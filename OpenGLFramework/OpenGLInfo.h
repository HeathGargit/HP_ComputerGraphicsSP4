/*---------------------------------------------------------
File Name: OpenGLInfo.h
Purpose: Stores buffer information for objectination
Author: Heath Parkes (gargit@gargit.net)
Modified: 1/12/2017
-----------------------------------------------------------
Copyright 2017 AIE/HP
---------------------------------------------------------*/

#ifndef OPENGLINFO
#define OPENGLINFO

struct OpenGLInfo
{
	unsigned int m_VAO; //vertex Array Object
	unsigned int m_VBO; //vertex Buffer Object
	unsigned int m_faceCount; //count of the faces in the model :D
};

#endif