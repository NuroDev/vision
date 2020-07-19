#pragma once

#include <DirectXMesh.h>
#include <fstream>
#include <iostream>
#include <utility>

#include "../Objects/WorldObject.h"
#include "Terrain/DiamondSquare.h"
#include "Terrain/Procedural.h"
#include "Terrain/StaticTerrain.h"

using namespace DirectX;

enum class TerrainType
{
	DiamondSquare,
	Procedural,
	Static,
};

class Terrain : public WorldObject
{
public:
	Terrain(
		std::string name,
		const unsigned int id,
		const TerrainType type,
		const char* heightMapPath = nullptr,
		Transform* pTransform = new Transform()
	) : WorldObject(name, id, pTransform), m_type(type), m_path(heightMapPath) {};
	
	~Terrain() { WorldObject::Release(); };

	/// [TERRAIN] Initialize the terrain and generate it
	HRESULT Init();

	/// [TERRAIN] Draw the terrain mesh to the screen
	void Draw();

	/// [TERRAIN] Draw the custom imgui ui to the screen
	void Ui();

	/// [TERRAIN] Generate a new terrain mesh with a specified width, depth & height
	HRESULT Generate() { return Generate(m_size, m_height); };

	/// [TERRAIN] Generate a new terrain mesh with a specified width, depth & height
	HRESULT Generate(const int size = Defaults::Terrain::Size, const float height = Defaults::Terrain::Height);

	/// [DIMENSIONS] Get the dimensions of the terrain
	[[nodiscard]] int GetDimensions() const { return m_size; };

	/// [DIMENSIONS] Set the dimensions of the terrain
	void SetDimensions(const int size) { m_size = size; };

	/// [DIMENSIONS] Get the height of the terrain
	[[nodiscard]] float GetHeight() const { return m_height; };

	/// [DIMENSIONS] Set the height of the terrain
	void SetHeight(float height) { m_height = height; };

	/// [TEXTURE] Load new texture from a specified path
	void SetTexture(const char* path) { m_texturePaths.push_back(path); };

	/// [TEXTURE] Loads all textures from an array of paths
	void SetTexture(std::vector<const char*> paths) { m_texturePaths = std::move(paths); };

private:
	/// [TERRAIN] Generate a new tri-quad grid mesh
	static Result<Mesh*, HRESULT> GenerateGrid(const int size);
	
	/// [TERRAIN] Calculate normals for a mesh
	static Result<std::vector<SimpleVertex>, HRESULT> CalculateNormals(std::vector<SimpleVertex> vertices, const size_t numVertices, std::vector<UINT> indices, const size_t numIndices);

	/// [TERRAIN] Load a single texture from a specified path into the generated mesh
	HRESULT LoadTexture(const char* path);
	
	/// [TERRAIN]
	static bool IsPowerOf2(const int val) { return (val & (val - 1)) == 0; }

	/// [TERRAIN]
	static int NextPowerOfTwo(int val)
	{
		--val;

		val |= val >> 1;
		val |= val >> 2;
		val |= val >> 4;
		val |= val >> 8;
		val |= val >> 16;

		return val + 1;
	}

	/// [TERRAIN] Micro function to handle ImGui radio buttons for setting noise types
	inline void SetType(const char* label, FastNoise::NoiseType type)
	{
		if (ImGui::RadioButton(label, m_fastNoiseTypeIndex == type))
		{
			m_fastNoiseTypeIndex = type;
			m_fastNoiseType = type;
			Generate(m_size, m_height);
		}
	};
	
	/// [DIMENSIONS]
	int m_size = Defaults::Terrain::Size;
	float m_height = Defaults::Terrain::Height;

	/// [HEIGHT MAP]
	const char* m_path = nullptr;
	
	/// [MESH]
	unsigned int m_numIndices = 0;
	std::vector<std::vector<float>> m_heightMap;

	/// [PARAMETERS]
	float m_scale = Defaults::Terrain::Scale;
	float m_frequency = Defaults::Terrain::Frequency;
	unsigned int m_octaves = Defaults::Terrain::Octaves;
	unsigned int m_seed = Defaults::Terrain::Seed;
	
	/// [SMOOTHING]
	bool m_doSmoothing = false;
	float m_smoothingFactor = 15.0f; // TODO: Convert to defaults value

	/// [TEXTURE]
	std::vector<const char*> m_texturePaths;
	std::vector<Texture*> m_pTextures;

	/// [TYPE]
	TerrainType m_type = TerrainType::Procedural;
	FastNoise::NoiseType m_fastNoiseType = FastNoise::NoiseType::Perlin;
	int m_fastNoiseTypeIndex = m_fastNoiseType;
};
