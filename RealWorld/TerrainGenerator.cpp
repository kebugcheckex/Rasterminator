#include "TerrainGenerator.h"

#include <iostream>
#include <ctime>
#include <noise/noise.h>
#include <vector>

#include "noiseutils.h"

using namespace noise;
using namespace std;

void TerrainGenerator::GaussianRandom(int size, std::vector< std::vector<double> >& output)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<> d;
	output = std::vector< std::vector<double> >(size, std::vector<double>(size, 0.0));
	for (int j = 0; j < size; j++)
		for (int i = 0; i < size; i++)
			output[i][j] = d(gen);
}

void TerrainGenerator::LowPassFilter(int size, double cutoff, int n, std::vector< std::vector<double> >& output)
{
	output = std::vector< std::vector<double> >(size, std::vector<double>(size, 0.0));
	for (int j = -size / 2; j < size / 2; j++)
	{
		for (int i = -size / 2; i < size / 2; i++)
		{
			double x = i / (double)(size);
			double y = j / (double)(size);
			double r = std::sqrt(x*x + y*y);
			double den = 1.0 + std::pow(r / cutoff, 2.0 * n);
			output[i + size / 2][j + size / 2] = 1.0 / den;
		}
	}
}

void TerrainGenerator::Convolution2D(std::vector< std::vector<double> >& data, std::vector< std::vector<double> >& filter, std::vector< std::vector<double> >& output)
{
	int size = data.size();
	int fsize = filter.size();
	output = std::vector< std::vector<double> >(size, std::vector<double>(size, 0.0));
	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			double sum = 0.0;
			for (int i = -fsize / 2; i < fsize / 2; i++)
			{
				for (int j = -fsize / 2; j < fsize / 2; j++)
				{
					int xindex = x + i;
					int yindex = y + j;

					if (xindex < 0) xindex = -xindex;
					if (xindex >= size) xindex = 2 * (size - 1) - xindex;
					if (yindex < 0) yindex = -yindex;
					if (yindex >= size) yindex = 2 * (size - 1) - yindex;

					sum += data[xindex][yindex] * filter[i + fsize / 2][j + fsize / 2];
				}
			}
			output[x][y] = sum;
		}
	}
}

void TerrainGenerator::DiamondSquare(std::vector< std::vector<double> >& terrainMap, int level, double terrainR, double roughness)
{

	std::random_device rdm;
	std::mt19937 engine(rdm());
	std::uniform_real_distribution<double> rnum(-1.0, 1.0);

	// Define The Height Map 2D Array
	int terrainSize = 1 << level;
	terrainMap = std::vector< std::vector<double> >(terrainSize + 1, std::vector<double>(terrainSize + 1, 0.0));

	// Initialize the Corners For Diamond Square
	terrainMap[0][0] = rnum(engine);
	terrainMap[0][terrainSize] = rnum(engine);
	terrainMap[terrainSize][0] = rnum(engine);
	terrainMap[terrainSize][terrainSize] = rnum(engine);

	double range = terrainR;

	// For each level;
	for (int lev = 0; lev < level; lev++) {

		int indLow = 1 << lev;
		int indHigh = 1 << (level - lev);
		int indMid = indHigh >> 1;

		// Calculate Diamonds
		for (int x = 0; x < terrainSize; x += indHigh) {
			for (int y = 0; y < terrainSize; y += indHigh) {
				if (indHigh > 1) {
					int halfPoint = indHigh / 2;
					terrainMap[x + halfPoint][y + halfPoint] = (terrainMap[x][y] + terrainMap[x + indHigh][y] + terrainMap[x][y + indHigh] + terrainMap[x + indHigh][y + indHigh]) / 4.0 + rnum(engine)*range;
				}
			}
		}

		// Calculate Squares
		if (indMid > 0) {
			for (int x = 0; x <= terrainSize; x += indMid) {
				for (int y = (x + indMid) % indHigh; y <= terrainSize; y += indHigh) {
					if (indHigh > 1) {
						int i = x - indMid;
						int j = y - indMid;
						int halfPoint = indHigh / 2;
						double meanVal = 0;
						int numPoints = 0;
						if (i >= 0) {
							meanVal = meanVal + terrainMap[i][j + halfPoint];
							numPoints++;
						}
						if (i + indHigh <= terrainSize) {
							meanVal = meanVal + terrainMap[i + indHigh][j + halfPoint];
							numPoints++;
						}
						if (j >= 0) {
							meanVal = meanVal + terrainMap[i + halfPoint][j];
							numPoints++;
						}
						if (j + indHigh <= terrainSize) {
							meanVal = meanVal + terrainMap[i + halfPoint][j + indHigh];
							numPoints++;
						}
						terrainMap[i + halfPoint][j + halfPoint] = meanVal / numPoints + rnum(engine)*range;
					}
				}
			}
		}

		// Update Range
		range = range*roughness;
	}
}

void TerrainGenerator::MultiFractal(std::vector<std::vector<double> >& terrainMap, int xSize, int ySize, double freq, double lacunarity) {

	module::RidgedMulti myModule;
	myModule.SetFrequency(freq);
	myModule.SetLacunarity(2.0);
	myModule.SetSeed(time(0));

	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule(myModule);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	heightMapBuilder.SetDestSize(xSize, ySize);
	heightMapBuilder.SetBounds(2.0, 6.0, 1.0, 5.0);
	heightMapBuilder.Build();

	terrainMap.resize(xSize, vector<double>(ySize, 0.0));
	for (int i = 0; i<xSize; i++) {
		for (int j = 0; j<ySize; j++) {
			terrainMap[i][j] = heightMap.GetValue(i, j) * 50.0f;
		}
	}
}

void TerrainGenerator::GenTriangles(std::vector< std::vector<double> >& heightMap,	// Input: height map
	std::vector<glm::vec3>& vertexList,		// Output: vertex array, each contains x, y, z
	std::vector<glm::vec3>& normalList,		// Output: normal array
	std::vector<glm::vec2>& textureList)	// Output: texture indices
{
	int size = heightMap.size();
	int steps = size - 1;
	int numTriangles = steps * steps * 2;
	vertexList = std::vector<glm::vec3>(numTriangles * 3);
	normalList = std::vector<glm::vec3>(numTriangles * 3);
	textureList = std::vector<glm::vec2>(numTriangles * 3);
	std::vector<Triangle> triangles(numTriangles);
	int tri = 0;
	for (int i = 0; i < steps; i++)
	{
		for (int j = 0; j < steps; j++)
		{
			triangles[tri++] = Triangle(i, j, i + 1, j, i, j + 1);
			triangles[tri++] = Triangle(i + 1, j, i + 1, j + 1, i, j + 1);
		}
	}
	std::vector< std::vector<glm::vec3> > map(steps + 1, std::vector<glm::vec3>(steps + 1));
	for (int i = 0; i <= steps; i++)
	{
		for (int j = 0; j <= steps; j++)
		{
			map[i][j].x = (i - steps / 2.0f)/(steps / 2.0f) * TRI_SCALE;
			map[i][j].z = (j - steps / 2.0f) / (steps / 2.0f) * TRI_SCALE;
			map[i][j].y = heightMap[i][j] * HEIGHT_SCALE;
		}
	}
	std::vector< std::vector<glm::vec3> > normals(steps + 1, std::vector<glm::vec3>(steps + 1));
	for (int i = 0; i < numTriangles; i++)
	{
		glm::vec3 v0 = map[triangles[i].x[0]][triangles[i].y[0]];
		glm::vec3 v1 = map[triangles[i].x[1]][triangles[i].y[1]];
		glm::vec3 v2 = map[triangles[i].x[2]][triangles[i].y[2]];
		glm::vec3 d1 = v0 - v1;
		glm::vec3 d2 = v1 - v2;
		glm::vec3 n = glm::cross(v0 - v1, v1 - v2);
		triangles[i].normal = n;
		for (int j = 0; j < 3; j++)
		{
			normals[triangles[i].x[j]][triangles[i].y[j]] += n;
		}
	}
	int counter = 0;
	for (int i = 0; i < map.size() - 1; i++)
	{
		for (int j = 0; j < map.size() - 1; j++)
		{
			vertexList[counter] = map[i][j];
			normalList[counter] = glm::normalize(normals[i][j]);
			textureList[counter].x = rand() / (double)RAND_MAX;
			textureList[counter].y = rand() / (double)RAND_MAX;
			vertexList[counter + 1] = map[i + 1][j];
			normalList[counter + 1] = glm::normalize(normals[i + 1][j]);
			textureList[counter + 1].x = rand() / (double)RAND_MAX;
			textureList[counter + 1].y = rand() / (double)RAND_MAX;
			vertexList[counter + 2] = map[i + 1][j + 1];
			normalList[counter + 2] = glm::normalize(normals[i + 1][j + 1]);
			textureList[counter + 2].x = rand() / (double)RAND_MAX;
			textureList[counter + 2].y = rand() / (double)RAND_MAX;
			counter += 3;
			vertexList[counter] = map[i][j];
			normalList[counter] = glm::normalize(normals[i][j]);
			textureList[counter].x = rand() / (double)RAND_MAX;
			textureList[counter].y = rand() / (double)RAND_MAX;
			vertexList[counter + 1] = map[i + 1][j + 1];
			normalList[counter + 1] = glm::normalize(normals[i + 1][j + 1]);
			textureList[counter + 1].x = rand() / (double)RAND_MAX;
			textureList[counter + 1].y = rand() / (double)RAND_MAX;
			vertexList[counter + 2] = map[i][j + 1];
			normalList[counter + 2] = glm::normalize(normals[i][j + 1]);
			textureList[counter + 2].x = rand() / (double)RAND_MAX;
			textureList[counter + 2].y = rand() / (double)RAND_MAX;
			counter += 3;
		}
	}
	float s = SKY_VERT / 2;
	glm::vec3 v0(s, s, s);
	glm::vec3 v1(s, s, -s);
	glm::vec3 v2(-s, s, -s);
	glm::vec3 v3(-s, s, s);
	glm::vec3 v4(s, -s, s);
	glm::vec3 v5(s, -s, -s);
	glm::vec3 v6(-s, -s, -s);
	glm::vec3 v7(-s, -s, s);
	
	std::vector<glm::vec3> sky;
}