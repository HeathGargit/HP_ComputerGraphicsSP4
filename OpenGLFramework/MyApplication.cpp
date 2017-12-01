/*---------------------------------------------------------
File Name: MyApplication.cpp
Purpose: The main "Program" part of the application. A lot of stuff could have been abstracted away into classes.
Author: Heath Parkes (gargit@gargit.net)
Modified: 1/12/2017
-----------------------------------------------------------
Copyright 2017 AIE/HP
---------------------------------------------------------*/

#include <gl_core_4_4.h>
#include "MyApplication.h"
#include <GLFW\glfw3.h>
#include <cstdio>
#include <Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Camera.h"
#include "Vertex.h"
#include "Shaderinator.h"
#include "OBJVertex.h"
#include "OpenGLInfo.h"
#include "tiny_obj_loader.h"
#include "Objectinator.h"
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

using namespace aie;
using glm::vec3;
using glm::vec4;
using glm::mat4;

MyApplication::MyApplication(const char* n)
{
	m_name = n;
	m_window = nullptr;
	m_camera = nullptr;
	m_VAO = 0;
	m_VBO = 0;
	m_IBO = 0;
}

bool MyApplication::startup()
{
	//check if the glfw works
		if (glfwInit() == false)
		{
			return false;
		}

	//code to do stuff in here
		m_window = glfwCreateWindow(1600, 900, m_name, nullptr, nullptr);

		//check that the return values are working
		if (m_window == nullptr)
		{
			glfwTerminate(); //shut down the graphics system
			return false; //return an error code
		}

	//make the window the current context
		glfwMakeContextCurrent(m_window);

	//do stuff to window
		if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
		{
			glfwDestroyWindow(m_window);
			glfwTerminate();
			return false;
		}

	//OpenGLFuncs are Now Available! Congrats!
		auto major = ogl_GetMajorVersion();
		auto minor = ogl_GetMinorVersion();
		printf("GL: %i.%i\n", major, minor);
	
	//setup imgui bindings
		ImGui_ImplGlfwGL3_Init(m_window, true);


	//these set up the camera
		//VIEW DEFINES WHERE THE CAMERA IS
		mat4 view = glm::lookAt(vec3(10.0f, 5.0f, 0.1f)/*where the camera is?*/, vec3(0, 1.0f, 0)/*what you're looking at*/, vec3(0, 1, 0)/*upwards direction*/);
		//PROJECITON DEFINES THE ANGLE OF THE VIEW
		mat4  projection = glm::perspective(glm::pi<float>() * 0.25f, 16.f / 9.f/*aspect ratio of the projection*/, 0.1f/*near clipping plane*/, 1000.f/*far clipping plane*/);
		//THESE ALWAYS HAVE TO BE MULTIPLIED IN THIS ORDER!!!! (otherwise it will look weird)

		m_camera = new Camera(m_window, glm::inverse(view));
		m_camera->setPerspecitve(glm::pi<float>() * 0.25f, 16.f / 9.f, 0.1f, 1000.f);

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		int success = GL_FALSE;

	//object shader program
		Shaderinator vertexShader("./shaders/week7vertexshader.txt", GL_VERTEX_SHADER);
		Shaderinator fragmentShader("./shaders/week7fragmentshader.txt", GL_FRAGMENT_SHADER);

		//create the shader "program" by attaching the tweo compiled shaders and linking them together
		m_programID = glCreateProgram();
		glAttachShader(m_programID, vertexShader.getShader());
		glAttachShader(m_programID, fragmentShader.getShader());
		glLinkProgram(m_programID);

		//compile the program(?)
		glGetProgramiv(m_programID, GL_LINK_STATUS, &success); //try to compile the program
		if (success == GL_FALSE) //check if program compiled successfully, if not
		{
			int infoLogLength = 0;
			glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
			char* infoLog = new char[infoLogLength];

			glGetProgramInfoLog(m_programID, infoLogLength, 0, infoLog);
			printf("Error: Failed to link shader Program!\n");
			printf("%s\n", infoLog);
			delete[] infoLog;
			return false;
		}

	//particle shader program
		Shaderinator particlevertexShader("./shaders/particle_emitter_tute_vertex.txt", GL_VERTEX_SHADER);
		Shaderinator particlefragmentShader("./shaders/particle_emitter_tute_fragment.txt", GL_FRAGMENT_SHADER);

		//create the shader "program" by attaching the tweo compiled shaders and linking them together
		m_ParticleProgramID = glCreateProgram();
		glAttachShader(m_ParticleProgramID, particlevertexShader.getShader());
		glAttachShader(m_ParticleProgramID, particlefragmentShader.getShader());
		glLinkProgram(m_ParticleProgramID);

		//compile the shader program
		glGetProgramiv(m_ParticleProgramID, GL_LINK_STATUS, &success); //try to compile the program
		if (success == GL_FALSE) //check if program compiled successfully, if not
		{
			int infoLogLength = 0;
			glGetProgramiv(m_ParticleProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
			char* infoLog = new char[infoLogLength];

			glGetProgramInfoLog(m_ParticleProgramID, infoLogLength, 0, infoLog);
			printf("Error: Failed to link shader Program!\n");
			printf("%s\n", infoLog);
			delete[] infoLog;
			return false;
		}

	// setup framebuffer
		glGenFramebuffers(1, &m_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
			glGenTextures(1, &m_FBOTexture);
			glBindTexture(GL_TEXTURE_2D, m_FBOTexture);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1600, 900);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
				GL_NEAREST);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				m_FBOTexture, 0);
			glGenRenderbuffers(1, &m_FBODepth);
			glBindRenderbuffer(GL_RENDERBUFFER, m_FBODepth);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
				1600, 900);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				GL_RENDERBUFFER, m_FBODepth);
			GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, drawBuffers);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// fullscreen quad
		glm::vec2 halfTexel = 1.0f / glm::vec2(1600, 900) * 0.5f;
		float vertexData[] = {
			-1, -1, 0, 1, halfTexel.x, halfTexel.y,
			1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,
			-1, 1, 0, 1, halfTexel.x, 1 - halfTexel.y,
			-1, -1, 0, 1, halfTexel.x, halfTexel.y,
			1, -1, 0, 1, 1 - halfTexel.x, halfTexel.y,
			1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,
		};
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);
			glGenBuffers(1, &m_VBO);
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 6,
				vertexData, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
				sizeof(float) * 6, 0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
				sizeof(float) * 6, ((char*)0) + 16);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//frame buffer shader prog
		Shaderinator FBvertexShader("./shaders/post_processing_tute_vertex.txt", GL_VERTEX_SHADER);
		Shaderinator FBfragmentShader("./shaders/post_processing_greyscale_fragment.txt", GL_FRAGMENT_SHADER);

		//create the shader "program" by attaching the tweo compiled shaders and linking them together
		m_FrameBufferProgramID = glCreateProgram();
		glAttachShader(m_FrameBufferProgramID, FBvertexShader.getShader());
		glAttachShader(m_FrameBufferProgramID, FBfragmentShader.getShader());
		glLinkProgram(m_FrameBufferProgramID);

		//compile the shader program
		glGetProgramiv(m_FrameBufferProgramID, GL_LINK_STATUS, &success); //try to compile the program
		if (success == GL_FALSE) //check if program compiled successfully, if not
		{
			int infoLogLength = 0;
			glGetProgramiv(m_FrameBufferProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
			char* infoLog = new char[infoLogLength];

			glGetProgramInfoLog(m_FrameBufferProgramID, infoLogLength, 0, infoLog);
			printf("Error: Failed to link shader Program!\n");
			printf("%s\n", infoLog);
			delete[] infoLog;

			return false;
		}

	//load objects
		Objectinator dias("./models/wow/scourge/sc_crystal_base.obj", "./models/wow/scourge/");
		m_Objects.push_back(dias);

		Objectinator crystal("./models/wow/magefocusingcrystal/mage_focusingcrystal_creature.obj", "./models/wow/magefocusingcrystal/");
		crystal.setWorldPos(glm::vec3(0, 2.5f, 0));
		m_Objects.push_back(crystal);

		Objectinator ground("./models/wow/floatingrocks/netherstorm_overhangrock_large_01.obj", "./models/wow/floatingrocks/");
		ground.setWorldPos(glm::vec3(0, -2.8f, 0));
		m_Objects.push_back(ground);

		Objectinator monolith("./models/wow/monolith/orcstoneburialpyre.obj", "./models/wow/monolith/");
		monolith.setWorldPos(glm::vec3(-5,-1.0f, 7));
		m_Objects.push_back(monolith);


	//particle emitter tute stuff
		m_Emitter = new ParticleEmitter();
		m_Emitter->initialise(1000, 50, 0.1f, 4.0f, 1, 5, 1, 1, glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 0));

	//set-up imgui variables for fiddling.
		useGreyscale = false;

	//setting up deltatime
		currentTime = (float)glfwGetTime();
		deltaTime = 0;
		m_TimePassed = 0;

	return true;
}

bool MyApplication::update()
{
	//if the window is told to close (by the close button?) or the escape key is pressed
	if (glfwWindowShouldClose(m_window) != false || glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		//return false to end the main app loop.
		return false;
	}

	//update deltatime
	float previousTime = currentTime;
	currentTime = (float)glfwGetTime();
	deltaTime = currentTime - previousTime;
	m_TimePassed += deltaTime;

	//particle emitter tute stuff
	m_Emitter->update(deltaTime, glm::inverse(m_camera->projectView()));

	//update the camera
	m_camera->update(deltaTime);

	//get inputs
	glfwPollEvents();

	//imgui stuff
	ImGui_ImplGlfwGL3_NewFrame();
		ImGui::Checkbox("Use Greyscale PostPro", &useGreyscale);
		ImGui::SliderInt("Rain Intensity", &m_Emitter->m_EmitRateAccessor, 0, 1000);
		ImGui::SliderFloat("Rain Direction X", &m_Emitter->m_direction.x, -3.0f, 3.0f);
		ImGui::SliderFloat("Rain Direction Z", &m_Emitter->m_direction.z, -3.0f, 3.0f);

	return true;
}

void MyApplication::draw()
{
	//draw to the target frame buffer first, then we can add post processing after
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glViewport(0, 0, 1600, 900);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //background colour
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //this cleares the screen and draws the background colour?

		glEnable(GL_DEPTH_TEST); //"enables the depth buffer" - something something how far things are drawn away from camera, like a draw order.

		glUseProgram(m_programID);

		//shader uniform set-ups.
		unsigned int projectionViewUniform = glGetUniformLocation(m_programID, "projectionViewWorldMatrix");
		glUniformMatrix4fv(projectionViewUniform, 1, GL_FALSE, glm::value_ptr(m_camera->projectView()));

		//glm::vec3 light_direction = glm::vec3(cos(m_rotation), 0, sin(m_rotation));
		glm::vec3 light_direction = glm::vec3(0, 3, 0);
		unsigned int lightDirection = glGetUniformLocation(m_programID, "lightDirection");
		glUniform3fv(lightDirection, 1, &light_direction[0]);

		glm::vec3 light_colour = glm::vec3(0.55f, 0.55f, 0.55f);
		unsigned int lightColour = glGetUniformLocation(m_programID, "lightColour");
		glUniform3fv(lightColour, 1, &light_colour[0]);

		glm::vec3 camera_position = m_camera->getPosition();
		unsigned int CameraPos = glGetUniformLocation(m_programID, "CameraPos");
		glUniform3fv(CameraPos, 1, &camera_position[0]);

		float specular_power = 5.0f;
		unsigned int SpecPow = glGetUniformLocation(m_programID, "SpecPow");
		glUniform1f(SpecPow, specular_power);

		unsigned int time = glGetUniformLocation(m_programID, "time");
		glUniform1f(time, m_TimePassed);


		//draw all your objects.
		for (auto object : m_Objects)
		{
			object.draw(m_programID);
		}

		glUseProgram(m_ParticleProgramID);

		//draw particles
		projectionViewUniform = glGetUniformLocation(m_ParticleProgramID, "projectionViewWorldMatrix");
		glUniformMatrix4fv(projectionViewUniform, 1, GL_FALSE, glm::value_ptr(m_camera->projectView()));

		m_Emitter->draw();

	//unbind the frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, 1600, 900);

	// draw our full-screen quad
	glUseProgram(m_FrameBufferProgramID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_FBOTexture);

	//set up uniform locations for post programs
	int loc = glGetUniformLocation(m_FrameBufferProgramID, "target");
	glUniform1i(loc, 0);
	bool shouldUseGreyscale = glGetUniformLocation(m_FrameBufferProgramID, "useGreyscale");
	glUniform1i(shouldUseGreyscale, useGreyscale);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//imgui
	ImGui::Render();

	//unbind the drawing thingo
	glBindVertexArray(0);

	glfwSwapBuffers(m_window); //double buffering
}

void MyApplication::shutdown()
{
	//shut down imgui
	ImGui_ImplGlfwGL3_Shutdown();

	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	//destroy the camera
	delete m_camera;

	//destroy the window context thing.
	glfwDestroyWindow(m_window);

	//terminate the glfw
	glfwTerminate();
}