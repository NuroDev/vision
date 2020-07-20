#pragma once

#define _USE_MATH_DEFINES

#include <d3d11.h>

#include "Config/Config.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Renderers/DeferredRenderer.hpp"
#include "Graphics/Renderers/PostProcessing.hpp"
#include "Graphics/Window.hpp"
#include "Libraries/imgui/imgui_impl_dx11.h"
#include "Libraries/imgui/imgui_impl_win32.h"
#include "Objects/Cube.hpp"
#include "Objects/Sphere.hpp"
#include "Objects/WorldLight.hpp"
#include "Objects/Terrain.hpp"
#include "Objects/WorldObject.hpp"
#include "Util/Resource.h"

class Application
{
public:
	Application(const char* path) : m_pConfig(new Config(path)) {};
	
	Application(Config* pConfig) : m_pConfig(pConfig) {};
	
	~Application() { Release(); };

	/// [APPLICATION] Initialize any core application data
	HRESULT Init(HINSTANCE hInstance, const int nCmdShow);

	/// [APPLICATION] Draw to the screen
	void Draw();

	/// [APPLICATION] Draw custom imgui content to screen
	void Ui();

	/// [APPLICATION] Update the application every tick
	void Update(float t);

	/// [APPLICATION] Cleanup & release the application data when done
	void Release();

private:
	/// [CONFIG]
	std::unique_ptr<Config> m_pConfig = nullptr;

	/// [OBJECTS]
	std::vector<WorldLight *> m_pLights;
	std::vector<WorldObject *> m_pWorldObjects;

	/// [RENDERERS]
	std::unique_ptr<DeferredRenderer> m_pDeferredRenderer = nullptr;
	std::unique_ptr<PostProcessing> m_pPostProcessing = nullptr;
};
