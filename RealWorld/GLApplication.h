#pragma once
#include <string>
#include <GL/glfw3.h>
#include <glm/glm.hpp>
class GLApplication
{
public:
	GLApplication(int xres, int yres);
	bool Init();
	//virtual ~GLApplication();
	virtual void OnKey(int key);
	virtual void RenderLoop() = 0;

	const std::string APP_NAME;
private:
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	int xres_, yres_;
	GLFWwindow *window_;
	glm::mat4 viewmat;
	glm::vec3 camera_pos;
	glm::vec3 look_at;
	glm::vec3 world_up;
	GLuint program_id;
	GLuint matrix_id;
};

