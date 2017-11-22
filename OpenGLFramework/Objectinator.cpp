#include "Objectinator.h"
#include "tiny_obj_loader.h"
#include "OBJVertex.h"
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

Objectinator::Objectinator(char * inputString)
{
	//tinyobject loading of object.
	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	bool model_load_success = tinyobj::LoadObj(&attribs, &shapes, &materials, &err, inputString);
	if (model_load_success)
	{
		createOpenGLBuffers(attribs, shapes);
	}
}

Objectinator::~Objectinator()
{
}

void Objectinator::draw()
{
	//draw all triangles in the object
	for (auto& gl : m_glInfo)
	{
		glBindVertexArray(gl.m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, gl.m_faceCount * 3);
	}
}

void Objectinator::createOpenGLBuffers(tinyobj::attrib_t& attribs, std::vector<tinyobj::shape_t>& shapes)
{
	m_glInfo.resize(shapes.size());

	//grab each shape
	int shapeIndex = 0;
	for (auto shape : shapes)
	{
		//Generate (allocate space?) for buffers
		glGenVertexArrays(1, &m_glInfo[shapeIndex].m_VAO);
		glGenBuffers(1, &m_glInfo[shapeIndex].m_VBO);

		glBindVertexArray(m_glInfo[shapeIndex].m_VAO); //binds the vertex array thingo. this starts the "recording" of stuff that happens.
		m_glInfo[shapeIndex].m_faceCount = shape.mesh.num_face_vertices.size();

		//collect triangle vertices
		std::vector<OBJVertex> vertices;
		int index = 0;
		for (auto face : shape.mesh.num_face_vertices)
		{
			for (int i = 0; i < 3; i++)
			{
				tinyobj::index_t idx = shape.mesh.indices[index + i];

				OBJVertex v = { 0 };
				//positions
				v.x = attribs.vertices[3 * idx.vertex_index + 0];
				v.y = attribs.vertices[3 * idx.vertex_index + 1];
				v.z = attribs.vertices[3 * idx.vertex_index + 2];
				//normals
				if (attribs.normals.size() > 0)
				{
					v.nx = attribs.normals[3 * idx.normal_index + 0];
					v.ny = attribs.normals[3 * idx.normal_index + 1];
					v.nz = attribs.normals[3 * idx.normal_index + 2];
				}
				//texture coords
				if (attribs.texcoords.size() > 0)
				{
					v.u = attribs.texcoords[2 * idx.texcoord_index + 0];
					v.v = attribs.texcoords[2 * idx.texcoord_index + 1];
				}

				vertices.push_back(v);
			}
			index += face;
		}

		//bind vertex data - this puts all the data into the buffer for the vid card.
		glBindBuffer(GL_ARRAY_BUFFER, m_glInfo[shapeIndex].m_VBO); //bind the vertex buffer for input
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(OBJVertex), vertices.data(), GL_STATIC_DRAW); //input the vertex data into the buffer

																											 //this sets up the first three floats of each vertex as the position
		glEnableVertexAttribArray(0); //position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(OBJVertex), 0);//this defines the position as three floats with offset of 0 form the start of the vertex

		glEnableVertexAttribArray(1); //normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(OBJVertex), (void*)offsetof(OBJVertex, nx));//this defines the normal as three floats with offset of 12 bytes form the start of the vertex

		glEnableVertexAttribArray(2); //texture data
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(OBJVertex), (void*)offsetof(OBJVertex, u));//this defines the texture map as two floats with offset of 24 bytes form the start of the vertex

		glBindVertexArray(0); //this "unbinds" the vertex array ("stops recording")
		glBindBuffer(GL_ARRAY_BUFFER, 0); //this unbinds the vertex buffer for input
		shapeIndex++;
	}
}
