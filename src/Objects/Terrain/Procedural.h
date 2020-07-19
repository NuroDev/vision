#pragma once

#include <vector>

#include "../../Libraries/FastNoise.h"

class Procedural
{
public:
	std::vector<std::vector<float>>Generate(const int size, const float frequency, const int octaves, const int seed, FastNoise::NoiseType type)
	{
		std::vector<std::vector<float>> m_heightMap(size * size);
		for (int i = 0; i < m_heightMap.size(); ++i)
			m_heightMap[i].resize(size * size);
		
		FastNoise noise;
		
		noise.SetNoiseType(type);
		noise.SetFrequency(frequency);
		noise.SetFractalOctaves(octaves);
		noise.SetSeed(seed);

		for (int x = 0; x < size; x++)
			for (int y = 0; y < size; y++)
				m_heightMap[x][y] = noise.GetNoise(static_cast<FN_DECIMAL>(x), static_cast<FN_DECIMAL>(y));

		return m_heightMap;
	};
};
