/*---------------------------------------------------------
File Name: Objectinator.h
Purpose: class that stores objects to be drawn on the screen. similar to a gameobject class.
Author: Heath Parkes (gargit@gargit.net)
Modified: 1/12/2017
-----------------------------------------------------------
Copyright 2017 AIE/HP
---------------------------------------------------------*/

#ifndef OBJECTINATOR
#define OBJECTINATOR

#include "OpenGLInfo.h"
#include "tiny_obj_loader.h"
#include <glm/vec3.hpp>

class Objectinator
{
public:
	Objectinator(char* inputString); //constructor for loading the obj untextured
	Objectinator(char* fileInputString, char* materialLocationInputString); //constructor for loading with texture
	~Objectinator();

	//program updates
	void draw(const unsigned int programID);

	//accessory type func for setting the world pos.
	void setWorldPos(glm::vec3 newPos);

private:
	//creates buffers for object to be re-rendered.
	void createOpenGLBuffers(tinyobj::attrib_t& attribs, std::vector<tinyobj::shape_t>& shapes, std::vector<tinyobj::material_t>& materials, const char* mat_loc);

	//member variables used in the class
	std::vector<OpenGLInfo> m_glInfo;
	unsigned int m_diffuse_material;
	unsigned int m_specular_material;
	unsigned int m_normal_material;

	//didnt get time to put this into a proper game object class so added world position here. cheap and cheery!
	glm::vec3 m_WorldPosition;
};

#endif