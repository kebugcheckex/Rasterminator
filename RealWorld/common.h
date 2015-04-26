/*
	This is a common header for this project
	So all the source code files can share some
	configuration information
*/

#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <GL/glfw3.h>
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#define FFT1
#define MULTI1
#define TERRAIN

#define PI 3.1415926535
#define CAM_POS 40.0f, -30.0f, 40.0f
#define LOOK_AT 0.0f, -15.0f, 0.0f
#define LIGHT_POS 0, -30, 0, 0
#define TRI_SCALE 40.0;
#define HEIGHT_SCALE 0.6f
#define SKY_VERT 40.0f