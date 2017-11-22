#include <gl_core_4_4.h>
#include "MyApplication.h"
#include <GLFW\glfw3.h>
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Camera.h"
#include "Vertex.h"
#include "Shaderinator.h"
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
	m_rows = 20;
	m_cols = 20;
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

	//these set up the camera
	//VIEW DEFINES WHERE THE CAMERA IS
	mat4 view = glm::lookAt(vec3(0, 20.0f, 0.1f)/*where the camera is?*/, vec3(20.0f, 0, 20.0f)/*what you're looking at*/, vec3(0, 1, 0)/*upwards direction*/);
	//PROJECITON DEFINES THE ANGLE OF THE VIEW
	mat4  projection = glm::perspective(glm::pi<float>() * 0.25f, 16.f / 9.f/*aspect ratio of the projection*/, 0.1f/*near clipping plane*/, 1000.f/*far clipping plane*/);
	//THESE ALWAYS HAVE TO BE MULTIPLIED IN THIS ORDER!!!! (otherwise it will look weird)

	m_camera = new Camera(m_window, glm::inverse(view));
	m_camera->setPerspecitve(glm::pi<float>() * 0.25f, 16.f / 9.f, 0.1f, 1000.f);
	//m_camera->setPerspecitve(projection);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int success = GL_FALSE;

	Shaderinator vertexShader("./shaders/week5vertexshader.txt", GL_VERTEX_SHADER);
	Shaderinator fragmentShader("./shaders/week5fragmentshader.txt", GL_FRAGMENT_SHADER);

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
	}

	//week 5 texture stuff
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	
	//use stbi to read the image file into an unsigned int
	unsigned char* data = stbi_load("./textures/floor.png", &imageWidth, &imageHeight, &imageFormat, STBI_default);

	//pass the loaded image to the graphics card through a handle
	glGenTextures(1, &m_texture); //the handle
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//unload the image
	stbi_image_free(data);

	//generate the surface to put the texture on.
	generateGround();

	//generateGrid(m_rows, m_cols);

	rotation = 0;
	currentTime = (float)glfwGetTime();
	deltaTime = 0;

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

	float previousTime = currentTime;
	currentTime = (float)glfwGetTime();
	deltaTime = currentTime - previousTime;

	rotation += deltaTime;
	m_camera->update(deltaTime);

	glfwPollEvents(); //get inputs

	return true;
}

void MyApplication::draw()
{
	//view = glm::lookAt(vec3(10 * cos(rotation), 10, 10 * sin(rotation)), vec3(0, 0, 0), vec3(0, 1, 0));

	glClearColor(0.4f, 0.4f, 0.4f, 1.0f); //background colour
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //this cleares the screen and draws the background colour?

	//glEnable(GL_DEPTH_TEST); //"enables the depth buffer" - something something how far things are drawn away from camera, like a draw order.

	glUseProgram(m_programID);
	
	unsigned int projectionViewUniform = glGetUniformLocation(m_programID, "projectionViewWorldMatrix");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(m_camera->projectView()));

	unsigned int time = glGetUniformLocation(m_programID, "time");
	glUniform1f(time, rotation);

	//week 5
	//set texture slot
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	//tell the shader where it is
	unsigned int loc = (glGetUniformLocation(m_programID, "diffuse"));
	glUniform1i(loc, 0);

	//draw
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	/*glBindVertexArray(m_VAO);
		unsigned int indexCount = (m_rows - 1) * (m_cols - 1) * 6;
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);*/

	glfwSwapBuffers(m_window); //double buffering
}

void MyApplication::shutdown()
{
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	//destroy the camera
	delete m_camera;

	//destroy the window context thing.
	glfwDestroyWindow(m_window);

	//terminate the glfw
	glfwTerminate();
}

void MyApplication::generateGrid(unsigned int rows, unsigned int cols)
{
	Vertex* aoVertices = new Vertex[rows * cols];
	for (unsigned int r = 0; r < rows; ++r)
	{
		for (unsigned int c = 0; c < cols; ++c)
		{
			aoVertices[r * cols + c].position = vec4((float)c, 0, (float)r, 1);
			//create some arbitrary colour based off something that might not be related to tiling a texture
			vec3 colour = vec3(sinf((c / (float)(cols - 1)) * (r / (float)(rows - 1))));
			aoVertices[r * cols + c].colour = vec4(colour, 1);
		}
	}

	unsigned int* auiIndicies = new unsigned int[(rows - 1) * (cols - 1) * 6];

	unsigned int index = 0;
	for (unsigned int r = 0; r < (rows - 1); ++r)
	{
		for (unsigned int c = 0; c < (cols - 1); ++c)
		{
			//triangle 1
			auiIndicies[index++] = r * cols + c;
			auiIndicies[index++] = (r + 1) * cols + c;
			auiIndicies[index++] = (r + 1) * cols + (c + 1);

			//triangle 2
			auiIndicies[index++] = r * cols + c;
			auiIndicies[index++] = (r + 1) * cols + (c + 1);
			auiIndicies[index++] = r * cols + (c + 1);
		}
	}

	//add the following line to generate a VertexArrayObject
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);
		
		glGenBuffers(1, &m_IBO);
		glGenBuffers(1, &m_VBO);
		//create and bind buffers to a vertex array object

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(Vertex), aoVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, colour));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 * sizeof(unsigned int), auiIndicies, GL_STATIC_DRAW);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] aoVertices;
	delete[] auiIndicies;
}

void MyApplication::generateGround()
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