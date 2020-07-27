#pragma once

#include "../Config/Defaults.hpp"

struct Effects
{
	Effects() :
		Blur(Defaults::Effects::BlurBox),
		Blur_Gaussian(Defaults::Effects::BlurGaussian),
		Blur_Samples(20),
		Blur_Size(0.03f),
		Blur_StandardDeviation(0.45f),
		ColorTint(Defaults::Effects::ColorTint),
		ColorTint_GrayScale(false),
		ColorTint_Invert(false),
		ColorTint_Color(XMFLOAT3(1.0f, 1.0f, 1.0f)),
		ColorTint_GameBoy(false),
		ColorTint_Sepia(false),
		ScreenWidth(Defaults::Application::Width),
		ScreenHeight(Defaults::Application::Height),
		Pixelate(Defaults::Effects::Pixelation),
		PixelationFactor(Defaults::Effects::PixelationFactor),
		PictureInPicture(Defaults::Effects::PictureInPicture),
		Vignette(Defaults::Effects::Vignette),
		VignetteAmount(Defaults::Effects::VignetteAmount),
		VignetteRadius(Defaults::Effects::VignetteRadius),
		VignetteRatio(Defaults::Effects::VignetteRatio),
		VignetteSlope(Defaults::Effects::VignetteSlope),
		ColorSpace(false),
		ChromaticAberration(false),
		ChromaticAberrationFactor(Defaults::Effects::ChromaticAberrationFactor)
	{}

	int Blur;
	int Blur_Gaussian;
	int Blur_Samples;
	float Blur_Size;
	float Blur_StandardDeviation;

	int ColorTint;
	int ColorTint_GrayScale;
	int ColorTint_Invert;
	XMFLOAT3 ColorTint_Color;
	int ColorTint_GameBoy;
	int ColorTint_Sepia;

	int Pixelate;
	float PixelationFactor;

	int Vignette;
	float VignetteAmount;
	float VignetteRadius;
	float VignetteRatio;
	int VignetteSlope;

	int ColorSpace;

	int ChromaticAberration;
	float ChromaticAberrationFactor;
	
	int PictureInPicture;
	unsigned int ScreenWidth;
	unsigned int ScreenHeight;

	float Padding[2];
};

struct EffectsBuffer
{
	Effects Effects;
};
