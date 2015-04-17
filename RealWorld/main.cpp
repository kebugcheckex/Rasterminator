// Include standard headers
#include "TerrainApp.h"


//int LoadObjects(string filename, GLfloat *vertices, GLfloat *normals, GLfloat *uv, GLfloat *colors)
//{
//	FILE *objfile = fopen(filename.c_str(), "r");
//	char dummy[256];
//	int index = 0;
//	while (fscanf(objfile, "%s", dummy) == 1)
//	{
//		fscanf(objfile, "%f %f %f %f %f %f %f %f",
//			&(vertices[index * 9]), &(vertices[index * 9 + 1]), &(vertices[index * 9 + 2]),
//			&(normals[index * 9]), &(normals[index * 9 + 1]), &(normals[index * 9 + 2]),
//			&(uv[index * 6]), &(uv[index * 6 + 1]));
//		fscanf(objfile, "%f %f %f %f %f %f %f %f",
//			&(vertices[index * 9 + 3]), &(vertices[index * 9 + 4]), &(vertices[index * 9 + 5]),
//			&(normals[index * 9 + 3]), &(normals[index * 9 + 4]), &(normals[index * 9 + 5]),
//			&(uv[index * 6 + 2]), &(uv[index * 6 + 3]));
//		fscanf(objfile, "%f %f %f %f %f %f %f %f",
//			&(vertices[index * 9 + 6]), &(vertices[index * 9 + 7]), &(vertices[index * 9 + 8]),
//			&(normals[index * 9 + 6]), &(normals[index * 9 + 0 + 7]), &(normals[index * 9 + 0 + 8]),
//			&(uv[index * 6 + 4]), &(uv[index * 6 + 5]));
//		index++;
//	}
//	for (int i = 0; i < index * 9; i++)
//	{
//		vertices[i] /= 2.5;
//	}
//	return index;	// How many triangles are there
//}

void mouse_move()
{
	/*static const double PI = 3.1415926535;
	static double cx = 1024 / 2;
	static double cy = 768 / 2;
	double cx_now, cy_now;
	glm::vec3 cam_dir = look_at - cam_pos;
	glfwGetCursorPos(window, &cx_now, &cy_now);
	double dx = cx_now - cx;
	double dy = cy_now - cy;
	double r = glm::length(cam_dir);
	double phi = dx / (1024 / 2) * PI / 4;
	look_at.x += r * sin(phi);
	double zx = r * (1 - cos(phi));
	double theta = -dy / (768 / 2) * PI / 4;
	look_at.y += r * sin(theta);
	double zy = r * (1 - cos(theta));
	look_at.z -= zx > zy ? zx : zy;
	View = glm::lookAt(cam_pos, look_at, glm::vec3(0, 1, 0));
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);*/
}
void key_callback()
{
	
	/*glm::vec3 cam_dir = look_at - cam_pos;
	cam_dir = glm::normalize(cam_dir);
	static glm::vec3 cam_left = glm::cross(world_up, cam_dir);
	cam_left = glm::normalize(cam_left);
	if (key == GLFW_KEY_Q)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}
	switch (key)
	{
	case GLFW_KEY_A:
		cam_pos += cam_left * 0.2f;
		look_at += cam_left * 0.2f;
		break;
	case GLFW_KEY_D:
		cam_pos -= cam_left * 0.2f;
		look_at -= cam_left * 0.2f;
		break;
	case GLFW_KEY_W:
		cam_pos += cam_dir * 0.2f;
		break;
	case GLFW_KEY_S:
		cam_pos -= cam_dir * 0.2f;
		break;
	case GLFW_KEY_UP:
		cam_pos += world_up * 0.2f;
		look_at += world_up * 0.2f;
		break;
	case GLFW_KEY_DOWN:
		cam_pos -= world_up * 0.2f;
		look_at -= world_up * 0.2f;
		break;
	case GLFW_KEY_LEFT:
		break;
	case GLFW_KEY_RIGHT:
		break;
	default:
		break;
	}
	View = glm::lookAt(cam_pos, look_at, glm::vec3(0, 1, 0));*/
}

int main(void)
{
	TerrainApp app(1024, 768);
	if (!app.Init()) exit(8);
	if (!app.InitShaders()) exit(9);
	if (!app.LoadTexture("texture.ppm")) exit(10);
	if (!app.LoadObject("cow.obj")) exit(11);
	app.RunRender();
	return 0;
}

