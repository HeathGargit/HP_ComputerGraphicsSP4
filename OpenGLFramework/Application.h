#ifndef APPLICATION
#define APPLICATION

#include "IApplication.h"
#include "Camera.h"
#include <GLFW\glfw3.h>

class Application : public IApplication
{
public:
	Application(const char*);
	bool startup()	override;
	bool update()	override;
	void draw()		override;
	void shutdown()	override;

private:
	GLFWwindow* m_window;
	const char* m_name;
	float m_time;
	float m_time_scale;
	glm::mat4 m_view;
	glm::mat4 m_projection;
	float rotation;
	float currentTime;
	float deltaTime;

	Camera* m_camera;
};

#endif