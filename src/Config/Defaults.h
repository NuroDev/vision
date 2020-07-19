#pragma once

#define _USE_MATH_DEFINES

#include <assimp/postprocess.h>
#include <DirectXCollision.h>
#include <math.h>

#include "../Util/Helpers.h"

using namespace DirectX;

struct Defaults
{
	struct Application
	{
		static const unsigned int Width = 1280;
		static const unsigned int Height = 720;
		static const bool ShowInspector = true;
		static const bool ShowMetrics = true;
	};

	struct Camera
	{
		static constexpr float AspectRatio = Defaults::Application::Width / static_cast<FLOAT>(Defaults::Application::Height);
		static constexpr float FOV = 0.25f * M_PI;
		static constexpr float FarZ = 1000.0f;
		static constexpr float NearZ = 1.0f;
		static constexpr XMFLOAT3 Position = XMFLOAT3(0.0f, 0.0f, -5.0f);
		static constexpr XMFLOAT3 Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
		static constexpr XMFLOAT3 Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		static constexpr XMFLOAT3 Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
		static constexpr float Speed = 5.0f;
	};

	struct Effects
	{
		static constexpr bool BlurBox = false;
		static constexpr bool BlurGaussian = false;
		static constexpr bool ColorTint = false;
		static constexpr bool PictureInPicture = false;
		static constexpr bool Pixelation = false;
		static constexpr bool Vignette = false;
		static constexpr float VignetteAmount = -1.0f;
		static constexpr float VignetteRadius = 1.0f;
		static constexpr float VignetteRatio = 1.0f;
		static constexpr int VignetteSlope = 8;
	};
	
	struct Graphics
	{
		static constexpr bool Deferred = false;
		static constexpr unsigned int DeferredNumBuffers = 2;
		static constexpr float ParallaxHeight = 0.0f;
		static constexpr int ParallaxMinSamples = 32;
		static constexpr int ParallaxMaxSamples = 64;
		static constexpr bool PostProcessing = false;
		static constexpr bool Transparency = false;
		static constexpr float TransparencyFactor = 0.75f;
		static constexpr bool UseTexture = true;
		static constexpr bool UseNormalMap = true;
		static constexpr bool UseParallaxMap = true;
		static constexpr bool UseParallaxOcclusion = false;
		static constexpr bool Wireframe = false;
		static constexpr bool Vsync = true;
	};

	struct Light
	{
		static constexpr bool Enabled = true;
		static constexpr float ConstantAttenuation = 1.0f;
		static constexpr XMFLOAT4 GlobalAmbient = XMFLOAT4(0.2f, 0.2f, 0.8f, 1.0f);
		static constexpr float SpotAngle = 0.45f;
		static constexpr float LinearAttenuation = 1;
		static constexpr float QuadraticAttenuation = 1;
		static constexpr unsigned int MaxLights = 1;
		static constexpr XMFLOAT3 Position = XMFLOAT3(0.0f, 0.75f, 0.0f);
		static constexpr XMFLOAT3 Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		static constexpr XMFLOAT4 Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	};

	struct Material
	{
		static constexpr XMFLOAT4 Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
		static constexpr XMFLOAT4 Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		static constexpr XMFLOAT4 Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		static constexpr XMFLOAT4 Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		static constexpr float SpecularPower = 32.0f;

		static constexpr int UseTexture = true;
		static constexpr int UseNormalMap = true;
		static constexpr int UseParallaxMap = false;

		static constexpr int UseParallaxOcclusion = false;
		static constexpr float ParallaxHeight = -0.05f;
		static constexpr int ParallaxMinSamples = 64;
		static constexpr int ParallaxMaxSamples = 128;
	};

	struct Model
	{
		static constexpr unsigned int Flags = aiProcessPreset_TargetRealtime_MaxQuality;
	};

	struct Terrain
	{
		static constexpr int Depth = 25;
		static constexpr float Height = 0.0f;
		static constexpr int Size = 25;
		static constexpr float Spacing = 0.0f;

		static constexpr float Frequency = 0.78f;
		static constexpr float Scale = 0.5f;
		static constexpr unsigned int Octaves = 8;
		static constexpr unsigned int Seed = 12345;
	};

	struct Transform
	{
		static constexpr XMFLOAT3 Position = XMFLOAT3(0.0f, 0.0, 0.0f);
		static constexpr XMFLOAT3 Rotation = XMFLOAT3(0.0f, 0.0, 0.0f);
		static constexpr XMFLOAT3 Scale = XMFLOAT3(1.0f, 1.0, 1.0f);
	};
};
