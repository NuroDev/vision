#include "DeferredBuffer.hpp"

HRESULT DeferredBuffer::Init()
{
	HRESULT hr = S_OK;

	// Get the graphics instance
	Graphics& graphics = Graphics::Get();

	// Initialize the render target texture
	D3D11_TEXTURE2D_DESC textureDesc = {};
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = m_resolution.x;
	textureDesc.Height = m_resolution.y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	for (int i = 0; i < Defaults::Graphics::DeferredNumBuffers; i++)
	{
		hr = graphics.GetDevice()->CreateTexture2D(&textureDesc, NULL, &m_pRenderTargetTextures[i]);
		if (FAILED(hr))
			return hr;
	}

	// Setup the description of the render target view
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target views.
	for (int i = 0; i < Defaults::Graphics::DeferredNumBuffers; i++)
	{
		hr = graphics.GetDevice()->CreateRenderTargetView(m_pRenderTargetTextures[i], &renderTargetViewDesc, &m_pRenderTargetViews[i]);
		if (FAILED(hr))
			return hr;
	}

	// Setup the description of the shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource views.
	for (int i = 0; i < Defaults::Graphics::DeferredNumBuffers; i++)
	{
		hr = graphics.GetDevice()->CreateShaderResourceView(m_pRenderTargetTextures[i], &shaderResourceViewDesc, &m_pShaderResourceViews[i]);
		if (FAILED(hr))
			return hr;
	}

	// Initialize the depth buffer
	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = m_resolution.x;
	depthBufferDesc.Height = m_resolution.y;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	hr = graphics.GetDevice()->CreateTexture2D(&depthBufferDesc, NULL, &m_pDepthStencilBuffer);
	if (FAILED(hr))
		return hr;

	// Initailze the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	hr = graphics.GetDevice()->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
	if (FAILED(hr))
		return hr;

	// Setup the viewport for rendering
	m_viewport.Width = static_cast<float>(m_resolution.x);
	m_viewport.Height = static_cast<float>(m_resolution.y);
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	return hr;
}

void DeferredBuffer::Ui()
{
	if (ImGui::CollapsingHeader("Deferred Rendering"))
	{
		for (unsigned int i = 0; i < Defaults::Graphics::DeferredNumBuffers; ++i)
		{
			ImGui::Image(static_cast<void*>(m_pShaderResourceViews[i]), ImVec2(288, 160));
		}
	}
}

void DeferredBuffer::Release()
{
	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(m_pDepthStencilBuffer);

	for (unsigned int i = 0; i < Defaults::Graphics::DeferredNumBuffers; i++)
	{
		SAFE_RELEASE(m_pShaderResourceViews[i]);
		SAFE_RELEASE(m_pRenderTargetViews[i]);
		SAFE_RELEASE(m_pRenderTargetTextures[i]);
	}
}

void DeferredBuffer::SetRenderTargets()
{
	// Get the graphics instance
	Graphics& graphics = Graphics::Get();
	
	// Bind the render target view array and depth stencil buffer to the output render pipeline.
	graphics.GetContext()->OMSetRenderTargets(Defaults::Graphics::DeferredNumBuffers, m_pRenderTargetViews, m_pDepthStencilView);

	// Set the viewport.
	graphics.GetContext()->RSSetViewports(1, &m_viewport);
}

void DeferredBuffer::ClearRenderTargets(const FLOAT color[4])
{
	// Get the graphics instance
	Graphics& graphics = Graphics::Get();
	
	// Clear all of the render target buffers
	for (int i = 0; i < Defaults::Graphics::DeferredNumBuffers; i++)
		graphics.GetContext()->ClearRenderTargetView(m_pRenderTargetViews[i], color);

	// Clear the depth buffer
	graphics.GetContext()->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
