#pragma once
#include "common.h"
class Triangle {
public:
	int x[3];
	int y[3];
	glm::vec3 normal;
	Triangle() {};
	Triangle(int i0, int j0, int i1, int j1, int i2, int j2)
	{
		x[0] = i0;
		y[0] = j0;
		x[1] = i1;
		y[1] = j1;
		x[2] = i2;
		y[2] = j2;
	}
};

class TerrainGenerator
{
public:
	static void GaussianRandom(int size, std::vector< std::vector<double> >& output);
	static void LowPassFilter(int size, double cutoff, int n, std::vector< std::vector<double> >& output);
	static void Convolution2D(std::vector< std::vector<double> >& data, std::vector< std::vector<double> >& filter, std::vector< std::vector<double> >& output);
	static void DiamondSquare(std::vector< std::vector<double> >& terrainMap, int level, double terrainR, double roughness);

	static void GenTriangles(std::vector< std::vector<double> >& heightMap,	// Input: height map
		std::vector<glm::vec3>& vertexList,		// Output: vertex array, each contains x, y, z
		std::vector<glm::vec3>& normalList,		// Output: normal array
		std::vector<glm::vec2>& textureList);
};

