#ifndef CAMERA
#define CAMERA


#include <glm/mat4x4.hpp>
#include <GLFW\glfw3.h>

class Camera
{
protected:
	glm::mat4 m_worldTransform;
	glm::mat4 m_viewTransform;
	glm::mat4 m_projectionTransform;
	glm::mat4 m_projectionViewTransform;
	GLFWwindow* m_window;
	float m_speed;
	bool m_dirty; //to tell if something needs to be updated.

	double m_last_x, m_last_y;

	double m_heading, m_pitch;

public:
	Camera(GLFWwindow*, const glm::mat4& world); //constructor
	void setPerspecitve(float fov, float aspect, float near, float far); //for a perspective camera, TO DO: see slides for orth config
	void setPerspecitve(glm::mat4 projectionTransform);
	void setView(const glm::mat4&);
	void setPosition(const glm::vec3& p);
	glm::vec3 getPosition();
	glm::vec3 getForward();
	glm::vec3 getUp();
	glm::vec3 getRight();

	const glm::mat4& worldTransform() const;
	const glm::mat4& projectView() const;
	const glm::mat4& projection() const;
	const glm::mat4& view() const;

	void setLookAt(const glm::vec3& from, const glm::vec3& to, const glm::vec3& up);
	void update(float deltatime);

	~Camera();
};

#endif