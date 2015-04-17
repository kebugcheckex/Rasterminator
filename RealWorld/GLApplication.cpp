#include <fstream>
#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

#include "GLApplication.h"

GLApplication::GLApplication(int xres, int yres)
	: APP_NAME("Rasterminator")
{
	xres_ = xres;
	yres_ = yres;
	fov_ = 45.0f;
	
	modelmat_ = glm::mat4(1.0f);

	camera_pos_ = glm::vec3(4.0f, 4.0f, 4.0f);
	look_at_ = glm::vec3(0.0f, 0.0f, 0.0f);
	world_up_ = glm::vec3(0.0f, 1.0f, 0.0f);
	viewmat_ = glm::lookAt(camera_pos_, look_at_, world_up_);

	projmat_ = glm::perspective(fov_, 4.0f / 3.0f, 0.1f, 100.0f);

}

bool GLApplication::Init()
{
#ifdef _DEBUG
	std::cout << "GLApplication is initializing...\n";
#endif /* _DEBUG */
	// Initialize GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to inintialize GLFW!\n";
		return false;
	}
	// Set some window properties
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window_ = glfwCreateWindow(xres_, yres_, APP_NAME.c_str(), NULL, NULL);
	if (!window_)
	{
		std::cerr << "Failed to create a window!\n";
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window_);
	glfwSetInputMode(window_, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(window_, GLApplication::key_callback);
	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW";
		return false;
	}

	

	// TODO set mouse scroll call back

	// Initialize OpenGL parameters
	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);

	// Not quite sure whether the following two lines should be placed here
	glGenVertexArrays(1, &varray_id);
	glBindVertexArray(varray_id);
	return true;
}

/*
	Currently shader source file names are hard-coded
	into the function.
*/
bool GLApplication::InitShaders()
{
	GLuint vertex_shader_id = load_shader("vertex_shader.glsl", GL_VERTEX_SHADER);
	if (vertex_shader_id == 0) return false;
	GLuint fragment_shader_id = load_shader("fragment_shader.glsl", GL_FRAGMENT_SHADER);
	if (fragment_shader_id == 0) return false;

	program_id = glCreateProgram();
	glAttachShader(program_id, vertex_shader_id);
	glAttachShader(program_id, fragment_shader_id);
	glLinkProgram(program_id);

	GLint result, info_len;
	
	glGetProgramiv(program_id, GL_LINK_STATUS, &result);
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_len);
	if (info_len > 0){
		char *message = new char[info_len + 1];
		glGetProgramInfoLog(info_len, info_len, NULL, message);
		std::cout << message << std::endl;
		delete[] message;
		return false;
	}

	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);

	matrix_id = glGetUniformLocation(program_id, "MVP");
	light_id = glGetUniformLocation(program_id, "LightPosition_worldspace");
	return true;
}

bool GLApplication::LoadTexture(const char *filename)
{
	texture = load_texture(filename);
	if (texture == 0) return false;
	texture_id = glGetUniformLocation(program_id, "myTextureSampler");
	return true;
}

bool GLApplication::LoadObject(const char *filename)
{
	if (!load_object(filename)) return false;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexList.size() * sizeof(glm::vec3), &vertexList[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvList.size() * sizeof(glm::vec2), &uvList[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normalList.size() * sizeof(glm::vec3), &normalList[0], GL_STATIC_DRAW);
	return true;
}

void GLApplication::RunRender()
{
	while (!glfwWindowShouldClose(window_))
	{
		RenderLoop();
	}
}
GLuint GLApplication::load_shader(const char *filename, GLenum shader_type)
{
	std::string source_code;
	std::ifstream source_file(filename, std::ios::in);
	if (source_file.is_open())
	{
		std::string line = "";
		while (getline(source_file, line))
			source_code += "\n" + line;
		source_file.close();
	}
	else
	{
		std::cerr << "Failed to load shader source code from " << filename << std::endl;
		return 0;
	}

	char const *source = source_code.c_str();
	GLuint shader_id = glCreateShader(shader_type);
	glShaderSource(shader_id, 1, &source, NULL);
	glCompileShader(shader_id);

	GLint result, info_len;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_len);
	if (info_len > 0){
		char *message = new char[info_len + 1];
		glGetShaderInfoLog(shader_id, info_len, NULL, message);
		std::cout << message << std::endl;
		delete[] message;
		return 0;
	}
	return shader_id;
}

GLuint GLApplication::load_texture(const char *filename)
{
	// TODO add code to handle bmp or other file types
	std::ifstream texture_file(filename, std::ios::in);
	if (!texture_file.is_open())
	{
		std::cerr << "Failed to open texture file " << filename << std::endl;
		return 0;
	}
	// Below is for ppm format
	std::string ppm_type;
	texture_file >> ppm_type;
	if (ppm_type != "P6")
	{
		std::cerr << "Unrecognized file format: " << ppm_type << std::endl;
		return 0;
	}
	unsigned int width, height, intensity;
	texture_file >> width >> height >> intensity;
	if (width > 1024 || height > 1024 || intensity > 4096)
	{
		std::cerr << "Invalid file parameters!\n";
		return 0;
	}
	unsigned char *image_data = new unsigned char[width*height * 3];
	texture_file.read((char*)image_data, width*height * 3);
	texture_file.close();

	GLuint textureid;
	glGenTextures(1, &textureid);
	glBindTexture(GL_TEXTURE_2D, textureid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, image_data);
	delete[] image_data;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	return textureid;
}

bool GLApplication::load_object(const char *filename)
{
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	FILE * file = fopen(filename, "r");
	if (file == NULL)
	{
		std::cout << "Failed to open the object file: " << filename << std::endl;
		return false;
	}

	while (true)
	{
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.
		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else
		{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}
	// For each vertex of each triangle
	for (unsigned int i = 0; i<vertexIndices.size(); i++)
	{
		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		vertexList.push_back(vertex);
		uvList.push_back(uv);
		normalList.push_back(normal);
	}
	return true;
}

void GLApplication::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}