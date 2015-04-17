#include "GLApplication.h"

#include <iostream>

#include <GL/glew.h>

GLApplication::GLApplication(int xres, int yres)
	: APP_NAME("Rasterminator")
{
}


GLApplication::~GLApplication()
{
}
bool GLApplication::Init()
{
#ifdef _DEBUG
	std::cout << "GLApplication is initializing...\n";
#endif /* _DEBUG */
	// Initialize GLFW
	if (!glfwInit()) return false;

	// Set some window properties
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create the window
	window_ = glfwCreateWindow(xres_, yres_, APP_NAME.c_str(), NULL, NULL);
	if (!window_)
	{
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window_);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) return false;

	glfwSetInputMode(window_, GLFW_STICKY_KEYS, GL_TRUE);

}
