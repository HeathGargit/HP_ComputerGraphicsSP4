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
	void createOpenGLBuffers(tinyobj::attrib_t& attribs, std::vector<tinyobj::shape_t>& shapes);

	std::vector<OpenGLInfo>		m_glInfo; //was abstracted into Objectinator - remove before final submission
	std::vector<Objectinator>	m_Objects;
	ParticleEmitter*			m_Emitter;

	glm::vec3					m_positions[2];
	glm::quat					m_rotations[2];

	//vertex buffer object stuff
	unsigned int				m_VAO;
	unsigned int				m_VBO;
	unsigned int				m_IBO;

	//frame buffer stuff
	unsigned int				m_FBO;
	unsigned int				m_FBOTexture;
	unsigned int				m_FBODepth;

	unsigned int				m_programID;
	unsigned int				m_ParticleProgramID;
	unsigned int				m_FrameBufferProgramID;

	GLFWwindow*					m_window;
	const char*					m_name;
	float						m_time;
	float						m_time_scale;
	glm::mat4					m_view;
	glm::mat4					m_projection;
	float						currentTime;
	float						deltaTime;
	float						m_rotation;

	Camera*						m_camera;
};

#endif

