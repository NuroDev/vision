#pragma once

#include <functional>

#include "../Graphics.hpp"
#include "DeferredBuffer.hpp"
#include "DeferredShader.hpp"
#include "LightShader.hpp"

class DeferredRenderer
{
public:
	DeferredRenderer() = default;

	DeferredRenderer(unsigned int width, unsigned int height) : m_resolution(XMINT2(width, height)) {};

	DeferredRenderer(XMINT2 resolution) : m_resolution(resolution) {};
	
	~DeferredRenderer() { Release(); };

	HRESULT Init()
	{
		HRESULT hr = S_OK;

		pDeferredBuffers = new DeferredBuffer();
		pDeferredBuffers->SetResolution(m_resolution);
		hr = pDeferredBuffers->Init();
		if (FAILED(hr))
			return hr;
		
		pDeferredShader = new DeferredShader();
		hr = pDeferredShader->LoadShader(L"Assets/Shaders/DeferredGeometry.fx");
		if (FAILED(hr))
			return hr;
		hr = pDeferredShader->Init();
		if (FAILED(hr))
			return hr;
		
		pLightShader = new LightShader();
		hr = pLightShader->LoadShader(L"Assets/Shaders/DeferredLighting.fx");
		if (FAILED(hr))
			return hr;
		hr = pLightShader->Init();
		if (FAILED(hr))
			return hr;
		
		return hr;
	};
	
	void Draw(const std::function<void()> render)
	{
		// Get graphics instance
		Graphics& graphics = Graphics::Get();
		
		// Swap the render targets to the render buffers & clear them
		pDeferredBuffers->SetRenderTargets();
		pDeferredBuffers->ClearRenderTargets(Colors::Black);

		// Update the constant buffer
		ConstantBuffer& constantBuffer = ConstantBuffer::Get();		
		constantBuffer.SetWorld(XMMatrixTranspose(constantBuffer.GetWorld()));
		graphics.GetContext()->UpdateSubresource(graphics.GetConstantBuffer(), 0, nullptr, &constantBuffer, 0, 0);

		// Draw the objects
		render();

		//pDeferredShader->Draw(constantBuffer, m_Model->GetIndexCount(), m_Model->GetTexture());

		graphics.SwapBackBuffer();
		graphics.ResetViewport();
	};

	void Ui()
	{
		pDeferredBuffers->Ui();
	}

	void Release()
	{
		SAFE_RELEASE(pDeferredBuffers);
		SAFE_RELEASE(pDeferredShader);
		SAFE_RELEASE(pLightShader);
	};

	/// [BUFFER]
	DeferredBuffer* pDeferredBuffers = nullptr;

	/// [SHADER]
	DeferredShader* pDeferredShader = nullptr;
	LightShader* pLightShader = nullptr;

private:
	/// [RESOLUTION]
	XMINT2 m_resolution = XMINT2(Defaults::Application::Width, Defaults::Application::Height);
};
