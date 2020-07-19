#pragma once

#include "../Config/Defaults.h"

struct Material
{
	Material() :
		Emissive(Defaults::Material::Emissive),
		Ambient(Defaults::Material::Ambient),
		Diffuse(Defaults::Material::Diffuse),
		Specular(Defaults::Material::Specular),
		SpecularPower(Defaults::Material::SpecularPower),
		UseTexture(Defaults::Material::UseTexture),
		UseNormalMap(Defaults::Material::UseNormalMap),
		UseParallaxMap(Defaults::Material::UseParallaxMap),
		UseParallaxOcclusion(Defaults::Material::UseParallaxOcclusion),
		ParallaxHeight(Defaults::Material::ParallaxHeight),
		ParallaxMinSamples(Defaults::Material::ParallaxMinSamples),
		ParallaxMaxSamples(Defaults::Material::ParallaxMaxSamples)
	{}

	XMFLOAT4 Emissive;
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	float SpecularPower;

	int UseTexture;
	int UseNormalMap;
	int UseParallaxMap;

	int UseParallaxOcclusion;
	float ParallaxHeight;
	int ParallaxMinSamples;
	int ParallaxMaxSamples;
};

struct MaterialBuffer
{
	Material Material;
};
