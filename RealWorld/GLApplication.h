#pragma once
#include <string>
#include <vector>
#include <GL/glew.h>
#include <GL/glfw3.h>
#include <glm/glm.hpp>

const double PI = 3.1415926535;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

class GLApplication
{
public:
	GLApplication(int xres, int yres);
	~GLApplication();
	bool Init();
	bool InitShaders();
	bool LoadTexture(const char *filename);
	bool LoadObject(const char *filename);
	void RunRender();
	void OnKey(int key);
	void RenderLoop();
	const std::string APP_NAME;
	
protected:
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
	glm::vec3 lightPos_;

	std::vector<glm::vec3> vertexList;
	std::vector<glm::vec3> normalList;
	std::vector<glm::vec2> uvList;

	GLuint varray_id;
	GLuint program_id;
	GLuint matrix_id;
	GLuint viewmat_id;
	GLuint modelmat_id;
	GLuint light_id;
	GLuint texture;
	GLuint texture_id;

	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;

	// Helper functions
	GLuint load_shader(const char *filename, GLenum shader_type);
	GLuint load_texture(const char *filename);
	bool load_object(const char *filename);
	bool load_terrain();
	GLuint load_dds(const char *filename);
	void mouse_move();
};

