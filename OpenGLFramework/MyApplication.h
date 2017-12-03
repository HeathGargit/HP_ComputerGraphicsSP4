/*---------------------------------------------------------
File Name: MyApplication.h
Purpose: The main "Program" part of the application. A lot of stuff could have been abstracted away into classes.
Author: Heath Parkes (gargit@gargit.net)
Modified: 1/12/2017
-----------------------------------------------------------
Copyright 2017 AIE/HP
---------------------------------------------------------*/

#ifndef MYAPPLICATION
#define MYAPPLICATION

#include "IApplication.h"
#include "Camera.h"
#include <GLFW\glfw3.h>
#include "tiny_obj_loader.h"
#include "OpenGLInfo.h"
#include "Objectinator.h"
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/quaternion.hpp>
#include "ParticleEmitter.h"

class MyApplication : public IApplication
{
public:
	MyApplication(const char*);
	bool startup()	override;
	bool update()	override;
	void draw()		override;
	void shutdown()	override;

private:
	//stuff for holding objects,emitters
	std::vector<Objectinator>	m_Objects;
	ParticleEmitter*			m_Emitter;

	//vertex buffer object stuff
	unsigned int				m_VAO;
	unsigned int				m_VBO;
	unsigned int				m_IBO;

	//frame buffer stuff
	unsigned int				m_FBO;
	unsigned int				m_FBOTexture;
	unsigned int				m_FBODepth;

	//shader program ids
	unsigned int				m_programID;
	unsigned int				m_ParticleProgramID;
	unsigned int				m_FrameBufferProgramID;
	bool						useGreyscale;

	GLFWwindow*					m_window; //gl window context thinger
	const char*					m_name; //name of the window
	glm::mat4					m_view; //camera variable
	glm::mat4					m_projection; //camera variable
	float						currentTime; //deltatime floats
	float						deltaTime; //deltatime
	float						m_TimePassed; //deltatime

	//lightning stuff
	bool						m_IsLightning;
	float						m_glCearColour;
	float						m_LightningTime;

	Camera*						m_camera; //the actual camera from the camera class
};

#endif

