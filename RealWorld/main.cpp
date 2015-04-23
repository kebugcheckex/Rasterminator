#include "GLApplication.h"
#include <iostream>
#include <random>
//class Triangle {
//public:
//	int x[3];
//	int y[3];
//	glm::vec3 normal;
//	//TODO: RGB
//	//TODO: color
//	Triangle() {};
//	Triangle(int i0, int j0, int i1, int j1, int i2, int j2)
//	{
//		x[0] = i0;
//		y[0] = j0;
//		x[1] = i1;
//		y[1] = j1;
//		x[2] = i2;
//		y[2] = j2;
//	}
//};
//
//void GaussianRandom(int size, std::vector< std::vector<double> >& output)
//{
//	std::random_device rd;
//	std::mt19937 gen(rd());
//	std::normal_distribution<> d;
//	output = std::vector< std::vector<double> >(size, std::vector<double>(size, 0.0));
//	for (int j = 0; j < size; j++)
//		for (int i = 0; i < size; i++)
//			output[i][j] = d(gen);
//}
//
//void LowPassFilter(int size, double cutoff, int n, std::vector< std::vector<double> >& output)
//{
//	output = std::vector< std::vector<double> >(size, std::vector<double>(size, 0.0));
//	for (int j = -size / 2; j < size / 2; j++)
//	{
//		for (int i = -size / 2; i < size / 2; i++)
//		{
//			double x = i / (double)(size);
//			double y = j / (double)(size);
//			double r = std::sqrt(x*x + y*y);
//			double den = 1.0 + std::pow(r / cutoff, 2.0 * n);
//			output[i + size / 2][j + size / 2] = 1.0 / den;
//		}
//	}
//}
//
//void Convolution2D(std::vector< std::vector<double> >& data, std::vector< std::vector<double> >& filter, std::vector< std::vector<double> >& output)
//{
//	int size = data.size();
//	int fsize = filter.size();
//	output = std::vector< std::vector<double> >(size, std::vector<double>(size, 0.0));
//	for (int x = 0; x < size; x++)
//	{
//		for (int y = 0; y < size; y++)
//		{
//			double sum = 0.0;
//			for (int i = -fsize / 2; i < fsize / 2; i++)
//			{
//				for (int j = -fsize / 2; j < fsize / 2; j++)
//				{
//					int xindex = x + i;
//					int yindex = y + j;
//
//					if (xindex < 0) xindex = -xindex;
//					if (xindex >= size) xindex = 2 * (size - 1) - xindex;
//					if (yindex < 0) yindex = -yindex;
//					if (yindex >= size) yindex = 2 * (size - 1) - yindex;
//
//					sum += data[xindex][yindex] * filter[i + fsize / 2][j + fsize / 2];
//				}
//			}
//			output[x][y] = sum;
//		}
//	}
//}
//
//void DiamondSquare(std::vector< std::vector<double> >& terrainMap, int level, double terrainR, double roughness) {
//
//	std::random_device rdm;
//	std::mt19937 engine(rdm());
//	std::uniform_real_distribution<double> rnum(-1.0, 1.0);
//
//	// Define The Height Map 2D Array
//	int terrainSize = 1 << level;
//	terrainMap = std::vector< std::vector<double> >(terrainSize + 1, std::vector<double>(terrainSize + 1, 0.0));
//
//	// Initialize the Corners For Diamond Square
//	terrainMap[0][0] = rnum(engine);
//	terrainMap[0][terrainSize] = rnum(engine);
//	terrainMap[terrainSize][0] = rnum(engine);
//	terrainMap[terrainSize][terrainSize] = rnum(engine);
//
//	double range = terrainR;
//
//	// For each level;
//	for (int lev = 0; lev < level; lev++) {
//
//		int indLow = 1 << lev;
//		int indHigh = 1 << (level - lev);
//		int indMid = indHigh >> 1;
//
//		// Calculate Diamonds
//		for (int x = 0; x < terrainSize; x += indHigh) {
//			for (int y = 0; y < terrainSize; y += indHigh) {
//				if (indHigh > 1) {
//					int halfPoint = indHigh / 2;
//					terrainMap[x + halfPoint][y + halfPoint] = (terrainMap[x][y] + terrainMap[x + indHigh][y] + terrainMap[x][y + indHigh] + terrainMap[x + indHigh][y + indHigh]) / 4.0 + rnum(engine)*range;
//				}
//			}
//		}
//
//		// Calculate Squares
//		if (indMid > 0) {
//			for (int x = 0; x <= terrainSize; x += indMid) {
//				for (int y = (x + indMid) % indHigh; y <= terrainSize; y += indHigh) {
//					if (indHigh > 1) {
//						int i = x - indMid;
//						int j = y - indMid;
//						int halfPoint = indHigh / 2;
//						double meanVal = 0;
//						int numPoints = 0;
//						if (i >= 0) {
//							meanVal = meanVal + terrainMap[i][j + halfPoint];
//							numPoints++;
//						}
//						if (i + indHigh <= terrainSize) {
//							meanVal = meanVal + terrainMap[i + indHigh][j + halfPoint];
//							numPoints++;
//						}
//						if (j >= 0) {
//							meanVal = meanVal + terrainMap[i + halfPoint][j];
//							numPoints++;
//						}
//						if (j + indHigh <= terrainSize) {
//							meanVal = meanVal + terrainMap[i + halfPoint][j + indHigh];
//							numPoints++;
//						}
//						terrainMap[i + halfPoint][j + halfPoint] = meanVal / numPoints + rnum(engine)*range;
//					}
//				}
//			}
//		}
//
//		// Update Range
//		range = range*roughness;
//	}
//}
//
//void GenTriangles(std::vector< std::vector<double> >& heightMap,	// Input: height map
//	std::vector<glm::vec3>& vertexList,		// Output: vertex array, each contains x, y, z
//	std::vector<glm::vec3>& normalList,		// Output: normal array
//	std::vector<glm::vec2>& textureList)	// Output: texture indices
//{
//	int size = heightMap.size();
//	int steps = size - 1;
//	int numTriangles = steps * steps * 2;
//	vertexList = std::vector<glm::vec3>(numTriangles * 3);
//	normalList = std::vector<glm::vec3>(numTriangles * 3);
//	textureList = std::vector<glm::vec2>(numTriangles * 3);
//	std::vector<Triangle> triangles(numTriangles);
//	int tri = 0;
//	for (int i = 0; i < steps; i++)
//	{
//		for (int j = 0; j < steps; j++)
//		{
//			triangles[tri++] = Triangle(i, j, i + 1, j, i, j + 1);
//			triangles[tri++] = Triangle(i + 1, j, i + 1, j + 1, i, j + 1);
//		}
//	}
//	std::vector< std::vector<glm::vec3> > map(steps + 1, std::vector<glm::vec3>(steps + 1));
//	for (int i = 0; i <= steps; i++)
//	{
//		for (int j = 0; j <= steps; j++)
//		{
//			map[i][j].x = i;
//			map[i][j].z = j;
//			map[i][j].y = heightMap[i][j];
//		}
//	}
//	std::vector< std::vector<glm::vec3> > normals(steps + 1, std::vector<glm::vec3>(steps + 1));
//	for (int i = 0; i < numTriangles; i++)
//	{
//		glm::vec3 v0 = map[triangles[i].x[0]][triangles[i].y[0]];
//		glm::vec3 v1 = map[triangles[i].x[1]][triangles[i].y[1]];
//		glm::vec3 v2 = map[triangles[i].x[2]][triangles[i].y[2]];
//		triangles[i].normal = glm::cross(v0 - v1, v1 - v2);
//		for (int j = 0; j < 0; j++)
//		{
//			normals[triangles[i].x[j]][triangles[i].y[j]] += triangles[i].normal;
//		}
//	}
//	int counter = 0;
//	for (int i = 0; i < map.size(); i++)
//	{
//		for (int j = 0; j < map.size(); j++)
//		{
//			vertexList[counter] = map[i][j];
//			normalList[counter] = normals[i][j];
//			std::cout << "x=" << vertexList[counter].x << "\t"
//				<< "y=" << vertexList[counter].y << "\t"
//				<< "z=" << vertexList[counter].z << std::endl;
//			textureList[counter].x = rand() / (double)RAND_MAX;
//			textureList[counter].y = rand() / (double)RAND_MAX;
//			counter++;
//		}
//	}
//}

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
	std::string texture_filename("wood.dds");
	std::string object_filename("teapot.object");
	if (argc > 2) texture_filename = argv[2];
	if (argc > 1) object_filename = argv[1];
	if (!app.Init()) exit(8);
	if (!app.InitShaders()) exit(9);
	if (!app.LoadTexture(texture_filename.c_str())) exit(10);
	if (!app.LoadObject(object_filename.c_str())) exit(11);
	app.RunRender();
	return 0;
}