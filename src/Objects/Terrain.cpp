#include "Terrain.h"

HRESULT Terrain::Init()
{
	HRESULT hr = S_OK;

	// Generate the intial terrain
	hr = Generate(m_size, m_height);
	if (FAILED(hr))
		return hr;

	// Initialize and load the textures
	for (const char* path : m_texturePaths)
	{
		hr = LoadTexture(path);
		if (FAILED(hr))
			return hr;
	}

	// Set the initial mesh textures
	for (Mesh* pMesh : GetMesh())
		pMesh->textures = m_pTextures;
	
	return hr;
}

void Terrain::Draw()
{
	// Get the graphics instance
	Graphics& graphics = Graphics::Get();

	// Set constant buffer world matrix
	ConstantBuffer& constantBuffer = ConstantBuffer::Get();
	constantBuffer.SetWorld(XMMatrixTranspose(GetWorld()));

	// Update constant buffer with new world matrix
	const ConstantBufferData constantBufferData = constantBuffer.GetData();
	graphics.GetContext()->UpdateSubresource(graphics.GetConstantBuffer(), 0, nullptr, &constantBufferData, 0, 0);

	// Set shaders
	graphics.GetContext()->VSSetShader(GetVertexShader(), nullptr, 0);
	graphics.GetContext()->PSSetShader(GetPixelShader(), nullptr, 0);

	// Set input layout
	graphics.GetContext()->IASetInputLayout(GetInputLayout());

	// Draw the model
	for (Mesh* pMesh : GetMesh())
		pMesh->Draw();
}

void Terrain::Ui()
{
	std::string fullname = GetName() + " (" + std::to_string(GetID()) + ")";

	if (ImGui::CollapsingHeader(fullname.c_str()))
	{
		for (Mesh* pMesh : GetMesh())
			if (ImGui::TreeNode("Material"))
			{
				Material material = pMesh->material;

				ImGui::ColorEdit4("Ambient", reinterpret_cast<float*>(&material.Ambient));
				ImGui::ColorEdit4("Diffuse", reinterpret_cast<float*>(&material.Diffuse));
				ImGui::ColorEdit4("Emissive", reinterpret_cast<float*>(&material.Emissive));
				ImGui::ColorEdit4("Specular", reinterpret_cast<float*>(&material.Specular));

				ImGui::SliderFloat("Specular Power", &material.SpecularPower, 0.0f, 128.0f);


				// TODO: Fix
				ImGui::Checkbox("Texture", reinterpret_cast<bool*>(&material.UseTexture));

				if (material.UseParallaxMap)
				{
					ImGui::Checkbox("Parallax Occlusion", reinterpret_cast<bool*>(&material.UseParallaxOcclusion));

					ImGui::InputFloat("Height", &material.ParallaxHeight, 0.01);

					if (ImGui::InputInt("Min Samples", &material.ParallaxMinSamples, 1, 10))
						if (material.ParallaxMinSamples <= 1)
							material.ParallaxMinSamples = 1;

					if (ImGui::InputInt("Max Samples", &material.ParallaxMaxSamples, 1, 10))
						if (material.ParallaxMaxSamples <= 1)
							material.ParallaxMaxSamples = 1;
				}

				pMesh->material = material;

				ImGui::TreePop();
			}
		
		if (ImGui::TreeNode("Parameters"))
		{
			ImGui::Checkbox("Enabled", &GetMesh(0)->enabled);

			if (m_type == TerrainType::Procedural)
			{
				if (ImGui::TreeNode("Noise Type"))
				{
					SetType("Value", FastNoise::NoiseType::Value);
					SetType("Value Fractal", FastNoise::NoiseType::ValueFractal);
					SetType("Perlin", FastNoise::NoiseType::Perlin);
					SetType("Perlin Fractal", FastNoise::NoiseType::PerlinFractal);
					SetType("Simplex", FastNoise::NoiseType::Simplex);
					SetType("Simplex Fractal", FastNoise::NoiseType::SimplexFractal);
					SetType("Cellular", FastNoise::NoiseType::Cellular);
					SetType("White Noise", FastNoise::NoiseType::WhiteNoise);
					SetType("Cubic", FastNoise::NoiseType::Cubic);
					SetType("Cubic Fractal", FastNoise::NoiseType::CubicFractal);

					ImGui::TreePop();
				}

				if (ImGui::InputInt("Size", &m_size, 2, 20)) Generate(m_size, m_height);
				if (ImGui::SliderFloat("Height", &m_height, 0.0f, 20.0f)) Generate(m_size, m_height);
				if (ImGui::SliderFloat("Scale", &m_scale, 0.0f, 5.0f)) Generate(m_size, m_height);
				if (ImGui::SliderFloat("Frequency", &m_frequency, 0.0f, 0.99f)) Generate(m_size, m_height);
				if (ImGui::InputInt("Octaves", reinterpret_cast<int*>(&m_octaves), 2, 20)) Generate(m_size, m_height);
				if (ImGui::InputInt("Seed", reinterpret_cast<int*>(&m_seed), 2, 20)) Generate(m_size, m_height);

				ImGui::Spacing();

				if (ImGui::Button("Random"))
				{
					m_seed = rand() % 4394967295 + 1;
					Generate(m_size, m_height);
				}

				ImGui::SameLine();

				if (ImGui::Button("Reset"))
					Generate(Defaults::Terrain::Size, Defaults::Terrain::Height);
			} else {
				if (ImGui::Checkbox("Smoothing", &m_doSmoothing))
					Generate(m_size, m_height);

				if (m_doSmoothing)
				{
					ImGui::SameLine();
					if (ImGui::SliderFloat("Smoothing Factor", &m_smoothingFactor, 0.01f, 50.0f))
						Generate(m_size, m_height);
				}

				if (ImGui::InputInt("Dimensions", &m_size, 2, 20))
					Generate(m_size, m_height);

				if (ImGui::SliderFloat("Height", &m_height, 0.0f, 20.0f))
					Generate(m_size, m_height);

				ImGui::Spacing();

				if (ImGui::Button("Generate"))
					Generate(m_size, m_height);

				ImGui::SameLine();

				if (ImGui::Button("Reset"))
					Generate(Defaults::Terrain::Size, Defaults::Terrain::Height);
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Textures"))
		{
			const size_t numTextures = m_pTextures.size();
			for (unsigned int i = 1; i < numTextures; ++i)
			{
				ImGui::Image(static_cast<void*>(m_pTextures[i - 1]->data), ImVec2(96, 96));
				if (i % 3)
					ImGui::SameLine();
			}

			ImGui::NewLine();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Transforms"))
		{
			Transform* pTransform = GetTransform();

			if (ImGui::TreeNode("Position"))
			{
				XMFLOAT3 currentPosition = pTransform->GetPosition();
				ImGui::SliderFloat("X", static_cast<float*>(&currentPosition.x), -10.0f, 10.0f);
				ImGui::SliderFloat("Y", static_cast<float*>(&currentPosition.y), -10.0f, 10.0f);
				ImGui::SliderFloat("Z", static_cast<float*>(&currentPosition.z), -10.0f, 10.0f);
				pTransform->SetPosition(currentPosition);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Rotation"))
			{
				XMFLOAT3 currentRotation = pTransform->GetRotation();
				ImGui::SliderFloat("X", static_cast<float*>(&currentRotation.x), 0.0f, 360.0f);
				ImGui::SliderFloat("Y", static_cast<float*>(&currentRotation.y), 0.0f, 360.0f);
				ImGui::SliderFloat("Z", static_cast<float*>(&currentRotation.z), 0.0f, 360.0f);
				pTransform->SetRotation(currentRotation);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Scale"))
			{
				XMFLOAT3 currentScale = pTransform->GetScale();
				ImGui::SliderFloat("X", static_cast<float*>(&currentScale.x), 0.1f, 10.0f);
				ImGui::SliderFloat("Y", static_cast<float*>(&currentScale.y), 0.1f, 10.0f);
				ImGui::SliderFloat("Z", static_cast<float*>(&currentScale.z), 0.1f, 10.0f);
				pTransform->SetScale(currentScale);
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}
	}
}

HRESULT Terrain::LoadTexture(const char* path)
{
	HRESULT hr = S_OK;
	
	const Result<Texture*, HRESULT> pTexture = Texture::Load(path);
	if (pTexture.isErr())
	{
		MessageBox(nullptr, L"[TERRAIN]\nFailed to load texture from texture. Please check all texture paths are valid", L"Error!", MB_ICONERROR | MB_OK);
		return E_FAIL;
	}	
	
	m_pTextures.push_back(pTexture.expect("Failed to extract texture loaded for terrain"));

	return hr;
}

HRESULT Terrain::Generate(int size, const float height)
{
	HRESULT hr = S_OK;

	// Store terrain data as members
	m_size = size;
	m_height = height;

	// If input goes below 0, set it to the lowest valid value, 4.
	if (size <= 0)
		size = 4;

	// If the size isn't valid, round it to the next power of 2
	if (!IsPowerOf2(size))
		size = NextPowerOfTwo(size);

	// ================================================================
	// Generate the grid
	// ================================================================
	Result<Mesh*, HRESULT> pMeshResult = GenerateGrid(size);
	if (pMeshResult.isErr())
		return E_FAIL;
	Mesh* pMesh = pMeshResult.expect("Failed to unwrap generated grid for terrain");

	// Apply the loaded textures
	pMesh->textures = m_pTextures;

	// Get the mesh vertices
	std::vector<SimpleVertex> vertices = pMesh->vertices;
	
	// ================================================================
	// Generate the height map
	// ================================================================
	switch (m_type)
	{
		// TODO: Fix static generator to return a vector<vector<float>>
		case TerrainType::Static:
		{
			// Project the grid into xz plane
			const float dx = static_cast<float>(size) / (size - 1);
			const float dz = static_cast<float>(size) / (size - 1);

			// Texture co-ordinates
			const float du = 1.0f / (size - 1);
			const float dv = 1.0f / (size - 1);

			// Load the heightmap data
			std::vector<float> heightMap = StaticTerrain().Generate(size * size, height, m_path);

			// Apply the height data to the vertices
			for (int i = 0; i < size; ++i)
				for (int j = 0; j < size; ++j)
				{
					const auto index = i * size + j;
					vertices[index].Position = XMFLOAT3(
						j * dx + (-size * 0.5f),		// X
						heightMap[index] * height,		// Y
						-(i * dz) + (size * 0.5f)		// Z
					);
					vertices[index].TexCoord = XMFLOAT2(j * du, i * dv);
				}
				
			break;
		}
		case TerrainType::DiamondSquare:
		case TerrainType::Procedural:
		{
			switch (m_type)
			{
			case TerrainType::DiamondSquare:
				m_heightMap = DiamondSquare().Generate(size);
				break;
			case TerrainType::Procedural:
				m_heightMap = Procedural().Generate(size, m_frequency, m_octaves, m_seed, m_fastNoiseType);
				break;
			}

			// Terrain dimensions cast to be unsigned
			const unsigned int uSize = static_cast<unsigned int>(size);

			// Apply the height map
			for (unsigned int i = 0; i < uSize; ++i)
				for (unsigned int j = 0; j < uSize; ++j)
					vertices[i * uSize + j].Position.y = ((m_heightMap[i][j] / 2.0f) * 1.0f) * (m_scale * -height);

			break;
		}
	}

	// ================================================================
	// Calculate normals
	// ================================================================
	std::vector<UINT> indices = pMesh->indices;
	Result<std::vector<SimpleVertex>, HRESULT> normalsResult = CalculateNormals(vertices, vertices.size(), indices, indices.size());
	if (normalsResult.isErr())
		return E_FAIL;
	vertices = normalsResult.expect("Failed to unwrap vertices vector after calculating normals");
	
	// ================================================================
	// Initialize the mesh
	// ================================================================
	pMesh->vertices = vertices;
	hr = pMesh->Init();
	if (FAILED(hr))
		return hr;

	// Set the world object mesh
	SetMesh({ pMesh });
	
	return hr;
}

Result<Mesh*, HRESULT> Terrain::GenerateGrid(const int size)
{
	// Terrain dimensions cast to be unsigned
	unsigned int m = static_cast<unsigned int>(size);
	unsigned int n = static_cast<unsigned int>(size);

	// Project the grid into xz plane
	const float dx = static_cast<float>(size) / (n - 1);
	const float dz = static_cast<float>(size) / (m - 1);

	// Texture co-ordinates
	const float du = 1.0f / (n - 1);
	const float dv = 1.0f / (m - 1);

	// ================================================================
	// Generate vertices
	// ================================================================
	std::vector<SimpleVertex> vertices(static_cast<size_t>(m * n));
	for (unsigned int i = 0; i < m; ++i)
		for (unsigned int j = 0; j < n; ++j)
		{
			const auto index = i * n + j;
			vertices[index].Position = XMFLOAT3(
				j * dx + (-size * 0.5f),	// X
				0.0f,						// Y
				-(i * dz) + (size * 0.5f)	// Z
			);
			vertices[index].TexCoord = XMFLOAT2(j * du, i * dv);
		}

	// ================================================================
	// Generate indices
	// ================================================================
	std::vector<UINT> indices(static_cast<size_t>(((m - 1) * (n - 1) * 2) * 3)); // Size of the number of faces * 3)
	unsigned int k = 0;
	for (unsigned int i = 0; i < m - 1; ++i)
		for (unsigned int j = 0; j < n - 1; ++j)
		{
			// AABC
			indices[k] = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1) * n + j;

			// ACBD
			indices[k + 3] = (i + 1) * n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1) * n + j + 1;

			// Next quad
			k += 6;
		}

	const size_t numVertices = vertices.size();
	const size_t numIndices = indices.size();

	// ================================================================
	// Calculate normals
	// ================================================================
	Result<std::vector<SimpleVertex>, HRESULT> normals = CalculateNormals(vertices, numVertices, indices, numIndices);
	if (normals.isErr())
		return Err(E_FAIL);
	vertices = normals.expect("Failed to unwrap vertices vector after calculating normals");

	return Ok(new Mesh(vertices, indices));
}

Result<std::vector<SimpleVertex>, HRESULT> Terrain::CalculateNormals(std::vector<SimpleVertex> vertices, const size_t numVertices, std::vector<UINT> indices, const size_t numIndices)
{
	HRESULT hr = S_OK;

	std::unique_ptr<XMFLOAT3[]> positions(new XMFLOAT3[numVertices]);
	std::unique_ptr<XMFLOAT3[]> normals(new XMFLOAT3[numVertices]);

	for (size_t i = 0; i < numVertices; ++i)
		positions[i] = vertices[i].Position;

	hr = ComputeNormals(indices.data(), numIndices / 3, positions.get(), numVertices, CNORM_DEFAULT, normals.get());
	if (FAILED(hr))
		return Err(hr);

	for (size_t i = 0; i < numVertices; ++i)
		vertices[i].Normal = normals[i];
	
	return Ok(vertices);
}
