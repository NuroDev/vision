#pragma once

#include <toml/toml.h>
#include <result.h>

#include "Structrures.hpp"
#include "../Camera/FirstPersonCamera.hpp"
#include "../Objects/Terrain.hpp"
#include "../Objects/Cube.hpp"
#include "../Objects/Plane.hpp"
#include "../Objects/Sphere.hpp"
#include "../Objects/WorldLight.hpp"

class Config
{
public:
	Config(const char* path);

	~Config() { Release(); };

	/// [CONFIG] Initialize the application using the new config data
	HRESULT Init();

	/// [CONFIG] Cleanup & release the application data when done
	void Release();

	/// [CONFIG] Load all objects from the config file
	HRESULT LoadScene();

	/// [CAMERA] Get a loaded camera from a specified index
	[[nodiscard]] FirstPersonCamera* GetCamera(const unsigned int index) const { return m_pCameras[index]; };

	/// [CAMERA] Get all loaded camera objects
	[[nodiscard]] std::vector<FirstPersonCamera*> GetCameras() const { return m_pCameras; };

	/// [CAMERA] Set a camera object at a specified index
	void SetCamera(FirstPersonCamera* pCamera, const unsigned int index) { m_pCameras[index] = pCamera; };

	/// [CAMERA] Set the cameras array
	void SetCameras(std::vector<FirstPersonCamera*> pCameras) { m_pCameras = std::move(pCameras); };

	/// [DATA] Get the data loaded from the config file
	[[nodiscard]] toml::Value Get() const { return m_data; };

	/// [DATA] Find a value from a specified key from the loaded config file
	const toml::Value* Get(const char* key) const { return m_data.find(key); };

	/// [DATA] Get the value from a specified key from the loaded config file
	template<typename T>
	T Get(const char* key) const { return m_data.get<T>(key); };

	/// [LIGHTS] Get a light from a specified index
	[[nodiscard]] WorldLight* GetLight(const unsigned int index) const { return m_pLights[index]; };

	/// [LIGHTS] Get all loaded lights
	[[nodiscard]] std::vector<WorldLight*> GetLights() const { return m_pLights; };

	/// [LIGHTS] Set a lights at a specified index
	void SetLight(WorldLight* pWorldLight, const unsigned int index) { m_pLights[index] = pWorldLight; };

	/// [LIGHTS] Set the lights array
	void SetLights(std::vector<WorldLight*> pWorldLights) { m_pLights = std::move(pWorldLights); };

	/// [OBJECTS] Get a world object from a specified index
	[[nodiscard]] WorldObject* GetWorldObject(const unsigned int index) const { return m_pWorldObjects[index]; };

	/// [OBJECTS] Get all loaded world objects
	[[nodiscard]] std::vector<WorldObject*> GetWorldObjects() const { return m_pWorldObjects; };

	/// [OBJECTS] Set a world object at a specified index
	void SetWorldObject(WorldObject* pWorldObject, const unsigned int index) { m_pWorldObjects[index] = pWorldObject; };

	/// [OBJECTS] Set the objects array
	void SetWorldObjects(std::vector<WorldObject*> pWorldObjects) { m_pWorldObjects = std::move(pWorldObjects); };

	/// [PARAMETERS] Get the application parameters
	[[nodiscard]] ApplicationParameters* GetAppParams() const { return m_pApplicationParams; };

	/// [PARAMETERS] Set the application parameters
	void SetAppParams(ApplicationParameters* pParams) { m_pApplicationParams = pParams; };

	/// [PARAMETERS] Get the graphics parameters
	[[nodiscard]] GraphicsParameters* GetGraphicsParams() const { return m_pGraphicsParams; };

	/// [PARAMETERS] Set the graphics parameters
	void SetGraphicsParams(GraphicsParameters* pParams) { m_pGraphicsParams = pParams; };

private:
	// TODO: Make type parameter an enum
	/// [CONFIG] Load in a new world object
	template<typename T>
	HRESULT LoadObject(const toml::Value& value, T* pObject, const std::string type);
	
	/// [PARAMETERS]
	ApplicationParameters* m_pApplicationParams = nullptr;
	GraphicsParameters* m_pGraphicsParams = nullptr;

	/// [CAMERA]
	std::vector<FirstPersonCamera*> m_pCameras;

	/// [DATA]
	toml::Value m_data;

	/// [LIGHTS]
	std::vector<WorldLight*> m_pLights;

	/// [OBJECTS]
	std::vector<WorldObject*> m_pWorldObjects;
};
