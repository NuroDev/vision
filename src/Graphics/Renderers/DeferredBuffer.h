#pragma once

#include "../Graphics.h"

class DeferredBuffer
{
public:
	~DeferredBuffer() { Release(); };

	/// [DEFERRED BUFFER] Initialize any core renderer data
	HRESULT Init();

	/// [DEFERRED BUFFER] Draw custom imgui Ui to the screen
	void Ui();
	
	/// [DEFERRED BUFFER] Cleanup & release the object when it is done
	void Release();

	/// [DEFERRED BUFFER] Set the current render target
	void SetRenderTargets();

	/// [DEFERRED BUFFER] Clear all render targets
	void ClearRenderTargets(const FLOAT color[4]);

	/// [RESOLUTION] Get the buffer resolution
	XMINT2 GetResolution() const { return m_resolution; };

	/// [RESOLUTION] Set the buffer resolution
	void SetResolution(unsigned int width, unsigned int height) { m_resolution = XMINT2(width, height); };

	/// [RESOLUTION] Set the buffer resolution
	void SetResolution(XMINT2 resolution) { m_resolution = resolution; };
	
private:
	/// [RESOLUTION]
	XMINT2 m_resolution;

	/// [RENDER TARGET]
	ID3D11RenderTargetView* m_pRenderTargetViews[Defaults::Graphics::DeferredNumBuffers];

	/// [STENCIL]
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
	
	/// [TEXTURE]
	ID3D11Texture2D* m_pRenderTargetTextures[Defaults::Graphics::DeferredNumBuffers];
	ID3D11ShaderResourceView* m_pShaderResourceViews[Defaults::Graphics::DeferredNumBuffers];
	ID3D11Texture2D* m_pDepthStencilBuffer = nullptr;

	/// [VIEWPORT]
	D3D11_VIEWPORT m_viewport;
};
