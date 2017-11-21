#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

void Camera::setLookAt(const glm::vec3 & from, const glm::vec3 & to, const glm::vec3 & up)
{
	m_viewTransform = glm::lookAt(from, to, glm::vec3(0, 1, 0));
	m_worldTransform = glm::inverse(m_viewTransform);
	m_projectionViewTransform = m_projectionTransform * m_viewTransform;
	m_dirty = true;
}

void Camera::update(float deltatime)
{
	//keyboard inputs
	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
	{
		auto p = getPosition() + getForward() * -m_speed * 10 * deltatime;
		setPosition(p);
	}
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
	{
		auto p = getPosition() + getForward() * m_speed * deltatime;
		setPosition(p);
	}
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
	{
		auto p = getPosition() + getRight() * m_speed * deltatime;
		setPosition(p);
	}
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
	{
		auto p = getPosition() + getRight() * -m_speed * deltatime;
		setPosition(p);
	}


	//We grab the current mouse position here
	//We only want the *change* in mouse position per frame though
	double x = 0, y = 0;
	glfwGetCursorPos(m_window, &x, &y);

	//m_last_? stores the last position of the mouse so we can see how much
	//the mouse has moved this frame
	//we don't know what the initial mouse position will be so we initialise
	//m_last_x to be -1 and if it's the first run through, set m_last_? to the current mouse position
	//to give an offset of (0,0)
	if (m_last_x == -1)
	{
		m_last_x = x;
		m_last_y = y;
	}

	//dx and dy are the differences in mouse position over the current frame
	const double dx = x - m_last_x;
	const double dy = y - m_last_y;

	//Now store the current cursor position to be checked against next frame
	m_last_x = x;
	m_last_y = y;

	//Rather than try to work directly with matrices we store a *heading* and a *pitch* angle
	//and build a new view matrix from those angles. The result is far more stable than concatenating matrices.
	m_heading -= dx*0.002f;
	m_pitch -= dy*0.002f;

	//We can clamp the pitch so the user can't go beyond looking straight up or straight down
	m_pitch = glm::clamp(m_pitch, -glm::half_pi<double>(), glm::half_pi<double>());

	//We calculate a rotation matrix around Y to define the heading, and a rotation matrix around X to define pitch
	//Multiply them together in the right order to make the final camera view transform
	auto roty = glm::rotate((float)m_heading, glm::vec3(0, 1.0f, 0));
	auto rotz = glm::rotate((float)m_pitch, glm::vec3(1, 0, 0));

	//Remember this is a world transform, our setView() must store the inverse of the input parameter in it's m_view matrix.
	this->setView(roty * rotz);


	/*//get the mouse position
	double x = 0, y = 0;
	glfwGetCursorPos(m_window, &x, &y);

	//sets to initial position first frame so there's no "kick" from the reading of the first position
	if (m_last_x == -1) 
	{
		m_last_x = x;
		m_last_y = y;
	}

	//get change
	const double dx = x - m_last_x;
	const double dy = y - m_last_y;

	//store current position for next frame calculation
	m_last_x = x;
	m_last_y = y;

	//change from original position
	m_heading -= dx * 0.002f;
	m_pitch -= dy * 0.002;

	//make sure you cant look up or down too far by clamping between half pi (radians?)
	m_pitch = glm::clamp(m_pitch, -glm::half_pi<double>(), glm::half_pi<double>()); 
	
	//calculate rotation
	auto roty = glm::rotate((float)m_heading, glm::vec3(0, 1, 0));
	auto rotz = glm::rotate((float)m_pitch, getRight());

	//apply rotation
	this->setView(roty * rotz);*/

	if (m_dirty)
	{
		m_projectionViewTransform = m_projectionTransform * m_viewTransform;
	}
	m_dirty = false;
}

Camera::~Camera()
{
}

Camera::Camera(GLFWwindow* window, const glm::mat4& world)
{
	m_window = window;
	m_worldTransform = world;
	m_speed = 1.0f;
	m_dirty = true;
	m_last_x = -1;
	m_last_y = -1;
	/*m_heading = glm::pi<double>();
	m_pitch = 0;*/

	//Use the 3rd vector of the 'world' matrix, which is the 'forward' direction vector, to determine heading and pitch through trigonometry.
	vec3 w(world[2]);
	vec3 a(0.0f, 0.0f, 1.0f);
	m_viewTransform = glm::inverse(world);
	//heading of 0 faces right, camera view matrix faces forwards so we offset by half_pi radians to compensate.
	m_heading = std::atan2(w.z, -w.x) - glm::half_pi<double>();
	float plane = sqrt(1.0f - w.y*w.y);
	m_pitch = -std::atan2(w.y, plane);
	//Build a heading rotation matrix and a pitch rotation matrix, and combine them into a single matrix by multiplying.
	auto roty = glm::rotate((float)m_heading, glm::vec3(0, 1.0f, 0)); //this is what we do every frame in the update. It's done here to set the initial pitch and "yaw"/heading
	auto rotz = glm::rotate((float)m_pitch, glm::vec3(1, 0, 0));    
	this->setView(roty * rotz);
}

void Camera::setPerspecitve(float fov, float aspect, float near, float far)
{
	m_projectionTransform = glm::perspective(fov, aspect, near, far);
	m_projectionViewTransform = m_projectionTransform * m_viewTransform;
	m_dirty = true;
}

void Camera::setPerspecitve(glm::mat4 projectionTransform)
{
	m_projectionTransform = projectionTransform;
	m_projectionViewTransform = m_projectionTransform * m_viewTransform;
	m_dirty = true;
}

void Camera::setView(const glm::mat4& mat)
{
	m_worldTransform = glm::translate(getPosition()) * mat;
	m_viewTransform = glm::inverse(m_worldTransform);
	m_dirty = true;
}

void Camera::setPosition(const glm::vec3 & p)
{
	m_worldTransform[3] = vec4(p, 1.f);
	m_viewTransform = glm::inverse(m_worldTransform);
	m_dirty = true;
}

glm::vec3 Camera::getPosition()
{
	auto p = worldTransform()[3];
	vec3 retv(p[0], p[1], p[2]);
	return retv;
}

glm::vec3 Camera::getForward()
{
	auto& f = m_worldTransform[2];
	return vec3(f.x, f.y, f.z);
}

glm::vec3 Camera::getUp()
{
	auto& f = m_worldTransform[1];
	return vec3(f.x, f.y, f.z);
}

glm::vec3 Camera::getRight()
{
	auto& f = m_worldTransform[0];
	return vec3(f.x, f.y, f.z);
}

const glm::mat4 & Camera::worldTransform() const
{
	return m_worldTransform;
}

const glm::mat4 & Camera::projectView() const
{
	return m_projectionViewTransform;
}

const glm::mat4 & Camera::projection() const
{
	return m_projectionTransform;
}

const glm::mat4 & Camera::view() const
{
	return m_viewTransform;
}
