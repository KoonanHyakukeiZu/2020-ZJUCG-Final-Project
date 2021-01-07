#ifndef TERRAIN_RANDOM_H
#define TERRAIN_RANDOM_H

#define PI 3.14159265

#include <random>
#include <math.h>
using namespace std;

namespace KooNan
{
	class HeightsGenerator {
	private:
		float amplitude;
		int seed;
		default_random_engine ran;
		float getSmoothNoise(int x, int z)
		{
			float corners = (getNoise(x - 1, z - 1) + getNoise(x + 1, z - 1) + getNoise(x - 1, z + 1) + getNoise(x + 1, z + 1)) / 16.0f;
			float sides = (getNoise(x - 1, z) + getNoise(x + 1, z) + getNoise(x, z - 1) + getNoise(x, z + 1)) / 8.0f;
			float center = getNoise(x, z) / 4.0f;
			return corners + sides + center;
		}
		float getInterpolatedNoise(float x, float z)
		{
			int intX = (int)x;
			int intZ = (int)z;
			float fracX = x - intX;
			float fracZ = z - intZ;

			float v1 = getSmoothNoise(intX, intZ);
			float v2 = getSmoothNoise(intX + 1, intZ);
			float v3 = getSmoothNoise(intX, intZ + 1);
			float v4 = getSmoothNoise(intX + 1, intZ + 1);
			float i1 = interpolate(v1, v2, fracX);
			float i2 = interpolate(v3, v4, fracX);
			return interpolate(i1, i2, fracZ);
		}
		float interpolate(float a, float b, float blend)
		{
			double theta = blend * PI;
			float f = (float)(1.0f - cos(theta)) * 0.5f;
			return a * (1.0f - f) + b * f;
		}
		float getNoise(int x, int z)
		{
			uniform_real_distribution<float> dist(0.0f, 1.0f);
			ran.seed(x * 25565 + z * 322222 + seed);
			return dist(ran)*2.0f - 1.0f;
		}
	public:
		HeightsGenerator(float amplitude = 8.0f, int seed = 21)
		{
			this->amplitude = amplitude;
			this->seed = seed;
		}
		float heightsGeneration(int x, int z)
		{
			return getInterpolatedNoise(x / 8.0f, z / 8.0f) *amplitude;
		}




	};
}
#endif