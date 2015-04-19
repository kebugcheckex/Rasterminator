#include <iostream>
#include <iomanip>
#include <random>
#include <cmath>
#include <vector>

void GaussianRandom(int size, std::vector< std::vector<double> >& output)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<> d;
	output = std::vector< std::vector<double> >(size, std::vector<double>(size, 0.0));
	for (int j = 0; j < size; j++)
		for (int i = 0; i < size; i++)
			output[i][j] = d(gen);
}

void LowPassFilter(int size, double cutoff, int n, std::vector< std::vector<double> >& output)
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

void Convolution2D(std::vector< std::vector<double> >& data, std::vector< std::vector<double> >& filter, std::vector< std::vector<double> >& output)
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

int main()
{
	const int size = 64;
	const int fsize = 32;
	std::vector< std::vector<double> > data;
	GaussianRandom(size, data);
	std::vector< std::vector<double> > filter;
	LowPassFilter(fsize, 0.25, 10, filter);
	std::vector< std::vector<double> > result(size, std::vector<double>(size, 0.0));
	return 0;
}