#ifndef MYAPPLICATION
#define MYAPPLICATION

#include "IApplication.h"
#include "Camera.h"
#include <GLFW\glfw3.h>
#include "tiny_obj_loader.h"
#include "OpenGLInfo.h"

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

	std::vector<OpenGLInfo> m_glInfo;

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	unsigned int m_programID;

	GLFWwindow* m_window;
	const char* m_name;
	float m_time;
	float m_time_scale;
	glm::mat4 m_view;
	glm::mat4 m_projection;
	float currentTime;
	float deltaTime;

	Camera* m_camera;
};

#endif

