#pragma once
#include <string>
#include <vector>
#include <GL/glfw3.h>
#include <glm/glm.hpp>
class GLApplication
{
public:
	GLApplication(int xres, int yres);
	bool Init();
	bool InitShaders();
	bool LoadTexture(const char *filename);
	bool LoadObject(const char *filename);
	//virtual ~GLApplication();
	virtual void OnKey(int key);
	virtual void RenderLoop() = 0;

	const std::string APP_NAME;
protected:
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	int xres_, yres_;
	GLFWwindow *window_;
	GLfloat fov_;

	glm::mat4 viewmat_;
	glm::mat4 projmat_;
	glm::mat4 modelmat_;
	glm::mat4 mvpmat_;
	glm::vec3 camera_pos_;
	glm::vec3 look_at_;
	glm::vec3 world_up_;

	std::vector<glm::vec3> vertexList;
	std::vector<glm::vec3> normalList;
	std::vector<glm::vec2> uvList;

	GLuint program_id;
	GLuint matrix_id;
	GLuint texture;
	GLuint texture_id;

	// Helper functions
	GLuint load_shader(const char *filename, GLenum shader_type);
	GLuint load_texture(const char *filename);
	bool load_object(const char *filename);
};

