#pragma once

#include <fstream>
#include <vector>

class StaticTerrain
{
public:
	std::vector<float> Generate(int size, float height, const char* path, bool smoothing = false, float smoothingFactor = 0.5f)
	{
		// Create temporary vector to store the loaded data
		std::vector<unsigned char> loadedData(size);

		// Open the file
		std::ifstream file(path, std::ios_base::binary);

		// Check the file is valid, if so then read the RAW bytes from it
		if (file)
		{
			file.read(reinterpret_cast<char*>(&loadedData[0]), static_cast<std::streamsize>(loadedData.size()));
			file.close();
		}

		// Store the loaded data in the global height map vector
		std::vector<float> heightMap(size, 1.0f);
		for (int i = 0; i < size; ++i)
		{
			heightMap[i] = (static_cast<float>(loadedData[i]) / 255.0f) * height;
			if (smoothing)
				heightMap[i] /= smoothingFactor;
		}

		// TODO: Take the size of the terrain image as an input and use it to scale the height data dynamically
		// by multiplying the grid height data based around that input size

		// Cleanup the temporary loaded data array
		loadedData.clear();
		
		return heightMap;
	};
};
