#pragma once

#include <windows.h>

#include "Defaults.hpp"

struct ApplicationParameters
{
	/// [INSPECTOR] Toggle whether to show the inspector window
	bool showInspector = Defaults::Application::ShowInspector;

	/// [METRICS] Show real-time application metrics
	bool showMetrics = Defaults::Application::ShowMetrics;

	/// [RESOLUTION] Width & height of the window to be created
	unsigned int width = Defaults::Application::Width;
	unsigned int height = Defaults::Application::Height;

	// TODO: Convert to applications default title string
	/// [TITLE] Windows/application title string
	std::string title = "Canvas";
};

struct CameraParameters
{
	/// [ASPECT RATIO] Camera's aspect ratio
	float aspectRatio = Defaults::Application::Width / static_cast<FLOAT>(Defaults::Application::Height);

	/// [FOV] Camera's field of view
	float farZ = Defaults::Camera::FarZ;
	float fovY = Defaults::Camera::FOV;
	float nearZ = Defaults::Camera::NearZ;
	
	/// [POSITION] Camera's initial starting position in 3D space
	XMFLOAT3 position = Defaults::Camera::Position;

	/// [ROTATION] Camera's initial starting rotation in 3D space
	XMFLOAT3 right = Defaults::Camera::Right;
	XMFLOAT3 up = Defaults::Camera::Up;
	XMFLOAT3 look = Defaults::Camera::Look;

	/// [SPEED] The movement speed of the camera
	float speed = Defaults::Camera::Speed;
};

struct GraphicsParameters
{
	/// [MISC] General graphical options
	bool doVsync = Defaults::Graphics::Vsync;

	/// [OBJECT] Custom object rendering properties
	bool doTransparency = Defaults::Graphics::Transparency;
	float doTransparencyFactor = Defaults::Graphics::TransparencyFactor;
	bool doWireframe = Defaults::Graphics::Wireframe;

	/// [RENDERERS] Custom renderer systems
	bool doDeferred = Defaults::Graphics::Deferred;
	bool doPostProcess = Defaults::Graphics::PostProcessing;
};

// TODO: Add initializer defaults
struct TerrainParameters
{
	/// [DIMENSIONS] Size dimensions of the terrain
	XMINT2 Dimensions = XMINT2(Defaults::Terrain::Size, Defaults::Terrain::Depth);
	
	/// [GRID SPACING] The sizing of the grid spacings
	float GridSpacing = Defaults::Terrain::Spacing;

	/// [HEIGHT] The value to scale the terrain by
	float HeightScale = Defaults::Terrain::Height;

	/// [PATH] Path to load the blending map
	const char* BlendingMapPath;
	
	/// [PATH] Path to load the height map
	const char* HeightMapPath;
};
