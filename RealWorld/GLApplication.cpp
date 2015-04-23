#include <fstream>
#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

#include "GLApplication.h"

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

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

GLApplication::~GLApplication()
{
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(program_id);
	glDeleteVertexArrays(1, &varray_id);
	glfwTerminate();
}

bool GLApplication::Init()
{
	if (!glfwInit())
	{
		std::cerr << "Failed to inintialize GLFW!\n";
		return false;
	}

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
	glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwSetKeyCallback(window_, key_callback);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW";
		return false;
	}

	// TODO set mouse scroll call back


	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);

	// Not quite sure whether the following two lines should be placed here
	glGenVertexArrays(1, &varray_id);
	glBindVertexArray(varray_id);

	texture_id = glGetUniformLocation(program_id, "myTextureSampler");
	return true;
}


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
	if (info_len > 10)
	{
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
	std::string filestr(filename);
	if (filestr.substr(filestr.size() - 3, 3) == "dds")
	{
		texture = load_dds(filename);
	}
	else if (filestr.substr(filestr.size() - 3, 3) == "ppm")
	{
		texture = load_texture(filename);
	}
	if (texture == 0) return false;
	
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

void GLApplication::RenderLoop()
{
	mouse_move();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program_id);
	mvpmat_ = projmat_ * viewmat_ * modelmat_;

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvpmat_[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(texture_id, 0);

	glm::vec3 lightPos = glm::vec3(4, 4, 4);
	glUniform3f(light_id, lightPos.x, lightPos.y, lightPos.z);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, vertexList.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	// Swap buffers
	glfwSwapBuffers(window_);
	glfwPollEvents();
}

/*
	Key control definitions:
	W	- move camera forward
	S	- move camera backward
	A	- move camera to the left
	D	- move camera to the right
	UP	- move camera upward
	DN	- move camera downward
*/
void GLApplication::OnKey(int key)
{
	glm::vec3 cam_dir = look_at_ - camera_pos_;
	cam_dir = glm::normalize(cam_dir);
	glm::vec3 cam_left = glm::cross(world_up_, cam_dir);
	cam_left = glm::normalize(cam_left);
	switch (key)
	{
	case GLFW_KEY_A:
		camera_pos_ += cam_left * 0.2f;
		look_at_ += cam_left * 0.2f;
		break;
	case GLFW_KEY_D:
		camera_pos_ -= cam_left * 0.2f;
		look_at_ -= cam_left * 0.2f;
		break;
	case GLFW_KEY_W:
		camera_pos_ += cam_dir * 0.2f;
		break;
	case GLFW_KEY_S:
		camera_pos_ -= cam_dir * 0.2f;
		break;
	case GLFW_KEY_UP:
		camera_pos_ += world_up_ * 0.2f;
		look_at_ += world_up_ * 0.2f;
		break;
	case GLFW_KEY_DOWN:
		camera_pos_ -= world_up_ * 0.2f;
		look_at_ -= world_up_ * 0.2f;
		break;
	default:
		break;
	}
	mvpmat_ = projmat_ * viewmat_ * modelmat_;
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
	if (info_len > 10){
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
			if (matches != 9)
			{
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

GLuint GLApplication::load_dds(const char *filename)
{
	unsigned char header[124];
	FILE *fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		std::cout << "Failed to load dds file: " << filename << std::endl;
		return 0;
	}

	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0)
	{
		fclose(fp);
		return 0;
	}

	fread(&header, 124, 1, fp);
	unsigned int height = *(unsigned int*)&(header[8]);
	unsigned int width = *(unsigned int*)&(header[12]);
	unsigned int linearSize = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC = *(unsigned int*)&(header[80]);

	unsigned char * buffer;
	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
	fread(buffer, 1, bufsize, fp);
	fclose(fp);

	unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
	unsigned int format;
	switch (fourCC)
	{
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		free(buffer);
		return 0;
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	/* load the mipmaps */
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
	{
		unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
			0, size, buffer + offset);
		offset += size;
		width /= 2;
		height /= 2;
		// Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
		if (width < 1) width = 1;
		if (height < 1) height = 1;

	}
	free(buffer);
	return textureID;
}

/*
	Mouse move 
*/
void GLApplication::mouse_move()
{
	float cx = xres_ / 2;
	float cy = yres_ / 2;
	double cx_now, cy_now;
	glm::vec3 cam_dir = look_at_ - camera_pos_;
	glfwGetCursorPos(window_, &cx_now, &cy_now);

	float dx = cx_now - cx;
	float dy = cy_now - cy;
	float r = glm::length(cam_dir);

	float phi = dx / (xres_ / 2) * PI / 4;
	look_at_.x += r * sin(phi);
	float zx = r * (1 - cos(phi));

	float theta = -dy / (yres_ / 2) * PI / 4;
	look_at_.y += r * sin(theta);
	float zy = r * (1 - cos(theta));

	look_at_.z -= zx > zy ? zx : zy;
	viewmat_ = glm::lookAt(camera_pos_, look_at_, world_up_);
	glfwSetCursorPos(window_, xres_ / 2, yres_ / 2);
}