#include "GLApplication.h"
#include <iostream>
#include <random>




GLApplication app(1024, 768);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_Q)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	else
	{
		app.OnKey(key);
	}
}

int main(int argc, char *argv[])
{
	std::string texture_filename("grass.dds");
	std::string object_filename("teapot.object");
	if (argc > 3) texture_filename = argv[2];
	if (argc > 2) object_filename = argv[1];
	if (!app.Init()) exit(8);
	if (!app.InitShaders()) exit(9);
	if (!app.LoadTexture(texture_filename.c_str())) exit(10);
	if (!app.LoadObject(object_filename.c_str())) exit(11);
	app.RunRender();
	return 0;
}