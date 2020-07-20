#pragma once

#include <vector>

class DiamondSquare
{
public:
	// TODO: Add input size value round up
	std::vector<std::vector<float>> Generate(int size, float persistence = 1.0f)
	{		
		m_size = size;
		m_persistance = persistence;

		// Initialize height map matrix with 0 zero values
		m_heightMap.resize(size * size);
		for (int i = 0; i < m_heightMap.size(); ++i)
			m_heightMap[i].resize(size);

		// Give all 4 corners a random value
		auto last = size - 1;
		m_heightMap[0][0] = GetOffset(size);
		m_heightMap[0][last] = GetOffset(size);
		m_heightMap[last][0] = GetOffset(size);
		m_heightMap[last][last] = GetOffset(size);

		Divide(m_size);
		
		return m_heightMap;
	};
	
private:
	void Divide(int stepSize)
	{
		int half = stepSize / 2;
		if (half < 1)
			return;

		// Square steps
		for (int z = half; z < m_size; z += stepSize)
			for (int x = half; x < m_size; x += stepSize)
				Square(x % m_size, z % m_size, half);

		// Diamond steps
		int col = 0;
		for (int x = 0; x < m_size; x += half)
		{
			col++;
			//If this is an odd column.
			if (col % 2 == 1)
				for (int z = half; z < m_size; z += stepSize)
					Diamond(x % m_size, z % m_size, half);
			else
				for (int z = 0; z < m_size; z += stepSize)
					Diamond(x % m_size, z % m_size, half);
		}

		Divide(stepSize / 2);
	}

	void Square(int x, int z, int reach)
	{
		int count = 0;
		float avg = 0.0f;

		const int xmr = x - reach;
		const int zmr = z - reach;
		const int xpr = x + reach;
		const int zpr = z + reach;
		
		if (x - reach >= 0 && z - reach >= 0)
		{
			avg += m_heightMap[xmr][zmr];
			count++;
		}
		if (x - reach >= 0 && z + reach < m_size)
		{
			avg += m_heightMap[xmr][zpr];
			count++;
		}
		if (x + reach < m_size && z - reach >= 0)
		{
			avg += m_heightMap[xpr][zmr];
			count++;
		}
		if (x + reach < m_size && z + reach < m_size)
		{
			avg += m_heightMap[xpr][zpr];
			count++;
		}

		avg += Random(reach);
		avg /= count;

		m_heightMap[x][z] = round(avg);
	}

	void Diamond(int x, int z, int reach)
	{
		int count = 0;
		float avg = 0.0f;

		const int xmr = x - reach;
		const int zmr = z - reach;
		
		if (x - reach >= 0)
		{
			avg += m_heightMap[x - reach][z];
			count++;
		}
		if (x + reach < m_size)
		{
			avg += m_heightMap[x + reach][z];
			count++;
		}
		if (z - reach >= 0)
		{
			avg += m_heightMap[x][z - reach];
			count++;
		}
		if (z + reach < m_size)
		{
			avg += m_heightMap[x][z + reach];
			count++;
		}

		avg += Random(reach);
		avg /= count;

		m_heightMap[x][z] = avg;
	}

	static float Random(const int range)
	{
		return static_cast<float>((rand() % range + -range));
	};
	
	float GetOffset(const int stepSize) const
	{
		auto offset = static_cast<float>(stepSize) / m_size * static_cast<int>(Random(m_size));
		auto sign = offset < 0 ? -1 : 1;
		return sign * static_cast<float>(pow(abs(offset), 1 / sqrt(m_persistance)));
	};

	float GetCellHeight(int x, int y, int size, int stepSize = 0)
	{
		if (x < 0 || y < 0 || x >= size || y >= size)
			return GetOffset(stepSize);
		
		return m_heightMap[x][y];
	}

	std::vector<std::vector<float>> m_heightMap;

	int m_size = 25;
	float m_persistance = 1.0f;
};
