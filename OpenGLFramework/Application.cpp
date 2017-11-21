#include <gl_core_4_4.h>
#include "Application.h"
#include <GLFW\glfw3.h>
#include <cstdio>
#include <Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Camera.h"

using namespace aie;
using glm::vec3;
using glm::vec4;
using glm::mat4;

Application::Application(const char* n)
{
	m_name = n;
	m_window = nullptr;
	m_camera = nullptr;
}

bool Application::startup()
{
	//check if the glfw works
	if (glfwInit() == false)
	{
		return false;
	}

	//code to do stuff in here
	m_window = glfwCreateWindow(1280, 720, m_name, nullptr, nullptr);

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

	Gizmos::create(1000, 1000, 1000, 1000);//gizmos is an AIE thing to make drawing primatives easy

										   //these set up the camera
										   //VIEW DEFINES WHERE THE CAMERA IS
	mat4 view = glm::lookAt(vec3(0, 3, -10)/*where the camera is?*/, vec3(0, 0, 0)/*what you're looking at*/, vec3(0, 1, 0)/*upwards direction*/);
	//PROJECITON DEFINES THE ANGLE OF THE VIEW
	mat4  projection = glm::perspective(glm::pi<float>() * 0.25f, 16.f / 9.f/*aspect ratio of the projection*/, 0.1f/*near clipping plane*/, 1000.f/*far clipping plane*/);
	//THESE ALWAYS HAVE TO BE MULTIPLIED IN THIS ORDER!!!! (otherwise it will look weird)

	m_camera = new Camera(m_window, glm::inverse(view));
	m_camera->setPerspecitve(glm::pi<float>() * 0.25f, 16.f / 9.f, 0.1f, 1000.f);
	//m_camera->setPerspecitve(projection);




	rotation = 0;
	currentTime = (float)glfwGetTime();
	deltaTime = 0;

	return true;
}

bool Application::update()
{
	//if the window is told to close (by the close button?) or the escape key is pressed
	if (glfwWindowShouldClose(m_window) != false || glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		//return false to end the main app loop.
		return false;
	}

	float previousTime = currentTime;
	currentTime = (float)glfwGetTime();
	deltaTime = currentTime - previousTime;

	rotation += deltaTime;
	m_camera->update(deltaTime);

	return true;
}

void Application::draw()
{
	//view = glm::lookAt(vec3(10 * cos(rotation), 10, 10 * sin(rotation)), vec3(0, 0, 0), vec3(0, 1, 0));

	glClearColor(0.4f, 0.4f, 0.4f, 1.0f); //background colour
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //this cleares the screen and draws the background colour?

	glEnable(GL_DEPTH_TEST); //"enables the depth buffer" - something something how far things are drawn away from camera, like a draw order.

	Gizmos::clear();
	Gizmos::addTransform(glm::mat4(1));

	//colours!
	const vec4 white(1);
	const vec4 black(0, 0, 0, 1);
	const vec4 sunYellow(1.0f, 0.8f, 0.0f, 1.0f);
	const vec4 green(0.0f, 0.65f, 0.12f, 1.0f);
	const vec4 grey(0.8f, 0.8f, 0.8f, 1.0f);

	//draw a grid
	for (int i = 1; i < 21; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, 10), vec3(-10 + i, 0, -10), i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i), vec3(-10, 0, -10 + i), i == 10 ? white : black);
	}
	//end of grid section

	//lets try and get a sun at least?
	vec3 planetPos(3 * cos(rotation), 0, 3 * sin(rotation));
	vec3 moonOffset(0.5 * cos(rotation * 10), 0, 0.5 * sin(rotation * 10));
	Gizmos::addSphere(vec3(0), 1.0f, 10, 10, sunYellow);
	Gizmos::addSphere(planetPos, 0.3f, 10, 10, green);
	Gizmos::addSphere(planetPos + moonOffset, 0.1f, 10, 10, grey);

	Gizmos::draw(m_camera->projection() * m_camera->view()); //must be in this order.
															 //Gizmos::draw(m_camera->projectView()); //must be in this order.


	glfwSwapBuffers(m_window); //double buffering
	glfwPollEvents(); //get inputs
}

void Application::shutdown()
{
	//destroy the camera
	delete m_camera;

	//destroy the gizmos
	Gizmos::destroy();
	//destroy the window context thing.
	glfwDestroyWindow(m_window);

	//terminate the glfw
	glfwTerminate();
}

