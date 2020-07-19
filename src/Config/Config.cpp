#include "Config.h"

Config::Config(const char* path)
{
	std::ifstream ifs(path);
	const toml::ParseResult data = toml::parse(ifs);

	// Check the data is valid
	if (!data.valid()) {
		MessageBox(nullptr, L"[CONFIG]\nConfig file loaded is not valid. Validate the file is valid TOML.", L"Error", MB_OK);
		exit(0);
	}

	// Store the loaded data
	m_data = data.value;

	// Intialize parameters objects
	if (m_pApplicationParams == nullptr)
		m_pApplicationParams = new ApplicationParameters();
	if (m_pGraphicsParams == nullptr)
		m_pGraphicsParams = new GraphicsParameters();
}

HRESULT Config::Init()
{
	HRESULT hr = S_OK;

	if (m_data.find("app.title"))
		m_pApplicationParams->title = m_data.get<std::string>("app.title");

	if (m_data.find("app.resolution"))
	{
		const std::vector<int> resolution = m_data.get<std::vector<int>>("app.resolution");
		m_pApplicationParams->width = resolution[0];
		m_pApplicationParams->height = resolution[1];
	}

	if (m_data.find("app.inspector"))
		m_pApplicationParams->showInspector = m_data.get<bool>("app.inspector");

	if (m_data.find("app.metrics"))
		m_pApplicationParams->showMetrics = m_data.get<bool>("app.metrics");

	// TODO: Add icon parameter

	std::map<std::string, toml::Value> app = m_data.get<toml::Table>("app");
	for (const auto& pValue : app)
	{
		std::string key = pValue.first;

		// TODO: Find a better way to pattern match the key string
		if (key == "camera")
		{
			CameraParameters* pCameraParams = new CameraParameters();

			if (pValue.second.find("aspect_ratio"))
				pCameraParams->aspectRatio = static_cast<float>(pValue.second.get<double>("aspect_ratio"));

			if (pValue.second.find("nearZ"))
				pCameraParams->nearZ = static_cast<float>(pValue.second.get<double>("nearZ"));

			if (pValue.second.find("fov"))
				pCameraParams->farZ = static_cast<float>(pValue.second.get<double>("fov"));

			FirstPersonCamera* pCamera = new FirstPersonCamera(pCameraParams);
			
			if (pValue.second.find("position"))
			{
				const std::vector<double> position = pValue.second.get<std::vector<double>>("position");
				pCameraParams->position = XMFLOAT3(static_cast<float>(position[0]), static_cast<float>(position[1]), static_cast<float>(position[2]));
			}

			if (pValue.second.find("rotation"))
			{
				const std::vector<double> rotation = pValue.second.get<std::vector<double>>("rotation");
				pCameraParams->right = XMFLOAT3(static_cast<float>(rotation[0]), 0.0f, 0.0f);
				pCameraParams->up = XMFLOAT3(0.0f, static_cast<float>(rotation[1]), 0.0f);
				pCameraParams->look = XMFLOAT3(0.0f, 0.0f, static_cast<float>(rotation[2]));
			}

			hr = pCamera->Init();
			if (FAILED(hr))
				return hr;

			m_pCameras.push_back(pCamera);
		}

		if (key == "graphics")
		{
			if (pValue.second.find("deferred"))
				m_pGraphicsParams->doDeferred = pValue.second.get<bool>("deferred");

			if (pValue.second.find("post_processing"))
				m_pGraphicsParams->doPostProcess = pValue.second.get<bool>("post_processing");

			if (pValue.second.find("transparency"))
				m_pGraphicsParams->doTransparency = pValue.second.get<bool>("transparency");

			if (pValue.second.find("transparency_factor"))
				m_pGraphicsParams->doTransparencyFactor = static_cast<float>(pValue.second.get<double>("transparency_factor"));

			if (pValue.second.find("vsync"))
				m_pGraphicsParams->doVsync = pValue.second.get<bool>("vsync");

			if (pValue.second.find("wireframe"))
				m_pGraphicsParams->doWireframe = pValue.second.get<bool>("wireframe");
		}
	}

	return hr;
}

void Config::Release()
{
	for (FirstPersonCamera* pCamera : m_pCameras)
		SAFE_RELEASE(pCamera);

	for (WorldLight* pLight : m_pLights)
		pLight->Release();

	for (WorldObject* pObject : m_pWorldObjects)
		SAFE_RELEASE(pObject);
}

HRESULT Config::LoadScene()
{
	HRESULT hr = S_OK;

	const toml::Array worldObjects = m_data.get<toml::Array>("scene.objects");
	
	for (const auto& pValue : worldObjects)
	{
		const std::string name = pValue.find("name") ? pValue.get<std::string>("name") : "Unnamed Object";
		const int id = pValue.find("id") ? pValue.get<int>("id") : rand() % 100;
		const std::string type = pValue.get<std::string>("type");

		if (type == "entity") {
			WorldObject* pObject = new WorldObject(name, id);
			
			hr = LoadObject<WorldObject>(pValue, pObject, type);
			if (FAILED(hr))
				return hr;

			m_pWorldObjects.push_back(pObject);			
		} else if (type == "cube") {
			Cube* pObject = new Cube(name, id);

			hr = LoadObject<Cube>(pValue, pObject, type);
			if (FAILED(hr))
				return hr;

			m_pWorldObjects.push_back(pObject);
		} else if (type == "plane") {
			Plane* pObject = new Plane(name, id);

			hr = LoadObject<Plane>(pValue, pObject, type);
			if (FAILED(hr))
				return hr;

			m_pWorldObjects.push_back(pObject);
		} else if (type == "sphere") {
			Sphere* pObject = new Sphere(name, id);

			hr = LoadObject<Sphere>(pValue, pObject, type);
			if (FAILED(hr))
				return hr;

			m_pWorldObjects.push_back(pObject);
		} else if (type == "terrain") {
			Terrain* pTerrain = new Terrain(name, id, TerrainType::Procedural);

			hr = LoadObject<WorldObject>(pValue, pTerrain, type);
			if (FAILED(hr))
				return hr;

			if (pValue.find("height"))
				pTerrain->SetHeight(static_cast<float>(pValue.get<double>("height")));

			m_pWorldObjects.push_back(pTerrain);
		} else if (type == "light") {
			WorldLight* pLight = new WorldLight(name, id);

			hr = LoadObject<WorldLight>(pValue, pLight, type);
			if (FAILED(hr))
				return hr;

			Light light = pLight->GetLight();

			if (pValue.find("color"))
			{
				const std::vector<double> color = pValue.get<std::vector<double>>("color");
				light.Color = XMFLOAT4(static_cast<float>(color[0]), static_cast<float>(color[1]), static_cast<float>(color[2]), static_cast<float>(color[3]));
			}

			if (pValue.find("light_enabled"))
				light.Enabled = pValue.get<bool>("light_enabled");

			if (pValue.find("light_type"))
			{
				const std::string lightTypeStr = pValue.get<std::string>("light_type");
				LightType lightType = LightType::PointLight;
				if (lightTypeStr == "directional")
					lightType = LightType::DirectionalLight;
				if (lightTypeStr == "point")
					lightType = LightType::PointLight;
				if (lightTypeStr == "spot")
					lightType = LightType::SpotLight;
				light.LightType = lightType;
			}

			pLight->SetLight(light);

			hr = pLight->Init();
			if (FAILED(hr))
				return hr;

			m_pLights.push_back(pLight);
		} else {
			MessageBox(nullptr, L"[CONFIG]\nNo valid object type found for an object. Please verify all objects have the correct object type name", L"Error", MB_OK);
			return E_FAIL;
		}
	}

	return hr;
}

template<typename T>
HRESULT Config::LoadObject(const toml::Value& value, T* pObject, const std::string type)
{
	HRESULT hr = S_OK;

	// Set whether the object is enabled by default
	if (value.find("enabled"))
		pObject->SetEnabled(value.get<bool>("enabled"));

	// Set the object id
	if (value.find("id"))
		pObject->SetID(value.get<int>("id"));
	
	// Load the model
	if (value.find("model"))
	{		
		hr = pObject->LoadModel(value.get<std::string>("model").c_str());
		if (FAILED(hr))
			return hr;
	}

	if (value.find("textures"))
	{
		// Get the textures paths
		const std::vector<std::string> paths = value.get<std::vector<std::string>>("textures");
		
		// Load the textures from file
		const Result<std::vector<Texture*>, HRESULT> loadedTextures = Texture::Load(paths);
		if (loadedTextures.isErr())
			return E_FAIL;

		// Store the newly loaded textures
		std::vector<Texture*> pTextures = loadedTextures.unwrap(); // Safe unwrap
		
		// TODO: Should check object for meshes rather than the config, or both
		if (!value.find("model")) {
			// Create vectors to store the initial primitive data
			std::vector<VertexPositionNormalTexture> primitiveVertices;
			std::vector<uint16_t> primitiveInidices;
			GeometricPrimitive::CreateSphere(primitiveVertices, primitiveInidices, pObject->GetTransform()->GetScale().x, 16, false);

			// Convert from geometric primitive vectors to custom mesh data
			const size_t numVertices = primitiveVertices.size();
			std::vector<SimpleVertex> vertices(numVertices);
			for (unsigned int i = 0; i < numVertices; ++i)
			{
				vertices[i].Position = primitiveVertices[i].position;
				vertices[i].Normal = primitiveVertices[i].normal;
				vertices[i].TexCoord = primitiveVertices[i].textureCoordinate;
			}
			const size_t numIndices = primitiveInidices.size();
			std::vector<UINT> indices(numIndices);
			for (unsigned int i = 0; i < numIndices; ++i)
				indices[i] = primitiveInidices[i];

			pObject->SetMesh({ new Mesh(vertices, indices, pTextures) });
		} else {
			// TODO: Does every mesh need to have every texture applied to it?
			for (Mesh* pMesh : pObject->GetMesh())
				for (Texture* pTexture : pTextures)
					pMesh->textures.push_back(pTexture);
		}
	}

	// Get the current transform object
	Transform* pTransform = pObject->GetTransform();

	if (value.find("position"))
	{
		const std::vector<double> position = value.get<std::vector<double>>("position");
		pTransform->SetPosition(static_cast<float>(position[0]), static_cast<float>(position[1]), static_cast<float>(position[2]));
	}

	if (value.find("rotation"))
	{
		const std::vector<double> rotation = value.get<std::vector<double>>("rotation");
		pTransform->SetRotation(static_cast<float>(rotation[0]), static_cast<float>(rotation[1]), static_cast<float>(rotation[2]));
	}

	if (value.find("scale"))
	{
		const std::vector<double> scale = value.get<std::vector<double>>("scale");
		pTransform->SetScale(static_cast<float>(scale[0]), static_cast<float>(scale[1]), static_cast<float>(scale[2]));
	}

	pObject->SetTransform(pTransform);

	// Load the shader
	if (value.find("shader"))
	{
		hr = pObject->LoadShader(value.get<std::string>("shader").c_str());
		if (FAILED(hr))
			return hr;
	}

	// Initialize the object
	hr = pObject->Init();
	if (FAILED(hr))
		return hr;

	return hr;
}
