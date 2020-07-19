#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "../Config/Defaults.h"

using namespace DirectX;

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
enum LightType
{
	DirectionalLight = 0,
	PointLight = 1,
	SpotLight = 2
};

// TODO: Move to dedicated light class
struct Light
{
	Light() :
		Position(Defaults::Light::Position.x, Defaults::Light::Position.y, Defaults::Light::Position.z, 1.0f),
		Direction(0.0f, 0.0f, 1.0f, 0.0f),
		Color(Defaults::Light::Color),
		SpotAngle(Defaults::Light::SpotAngle),
		ConstantAttenuation(Defaults::Light::ConstantAttenuation),
		LinearAttenuation(Defaults::Light::LinearAttenuation),
		QuadraticAttenuation(Defaults::Light::QuadraticAttenuation),
		LightType(DirectionalLight),
		Enabled(0),
		Padding{}
	{}

	XMFLOAT4 Position;
	XMFLOAT4 Direction;
	XMFLOAT4 Color;

	float SpotAngle;
	float ConstantAttenuation;
	float LinearAttenuation;
	float QuadraticAttenuation;
	int LightType;
	int Enabled;
	int Padding[2];
};

struct LightBuffer
{
	LightBuffer() :
		EyePosition(Defaults::Light::Position.x, Defaults::Light::Position.y, Defaults::Light::Position.z, 1),
		GlobalAmbient(Defaults::Light::GlobalAmbient)
	{}

	XMFLOAT4 EyePosition;
	XMFLOAT4 GlobalAmbient;
	Light Lights[Defaults::Light::MaxLights];
};
