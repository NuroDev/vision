#pragma once

#include <CommonStates.h>
#include <functional>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>

#include "../Graphics.hpp"
#include "../../Graphics/Effect.hpp"

using namespace DirectX;

class PostProcessing
{
public:
	PostProcessing() = default;

	PostProcessing(const unsigned int width, const unsigned int height) { m_effects.ScreenWidth = width; m_effects.ScreenHeight= height; };

	PostProcessing(Effects effects) : m_effects(effects) {};
	
	~PostProcessing() { Release(); };

	/// [POST-PROCESSING] Initialize any core renderer data
	HRESULT Init();

	/// [POST-PROCESSING] Draw the contents of the renderer to the screen
	void Draw(std::function<void()> render);

	/// [POST-PROCESSING] Draw the custom imgui ui to the screen
	void Ui();

	/// [POST-PROCESSING] Update the object every tick
	void Update(float t);

	/// [POST-PROCESSING] Cleanup & release the object when it is done
	void Release();

	/// [EFFECTS] Get the special effects properties
	Effects GetEffects() const { return m_effects; };

	/// [EFFECTS] Set the special effects properties object
	void SetEffects(Effects effects) { m_effects = effects; };

	/// [RENDER TARGET] Get the off-screen post-processing render target
	[[nodiscard]] ID3D11RenderTargetView *GetRenderTargetView() const { return m_pRenderTargetView; };

	/// [RENDER TARGET] Swap to an alternate render target
	void SwapRenderTarget(ID3D11RenderTargetView *pRenderTargetView, ID3D11DepthStencilView *pDepthStencilView, const FLOAT colors[4]);

	/// [RESOLUTION] Set the resolution
	void SetResolution(const unsigned int width, const unsigned int height)
	{
		m_effects.ScreenWidth = width;
		m_effects.ScreenHeight = height;
	};

	/// [SHADER] Get the shader resource view
	[[nodiscard]] ID3D11ShaderResourceView *GetShaderResourceView() const { return m_pSRV; };

	/// [SHADER] Load a shader from a specified file
	HRESULT LoadShader(const WCHAR *path, LPCSTR vertexEntry = "VS", LPCSTR pixelEntry = "PS");

private:
	/// [BUFFER]
	ID3D11Buffer* m_pEffectsConstantBuffer = nullptr;

	/// [EFFECTS]
	Effects m_effects;

	/// [QUAD]
	std::unique_ptr<PrimitiveBatch<VertexPositionTexture>> m_pQuad;
	std::vector<VertexPositionTexture> m_vertices;
	std::vector<uint16_t> m_indices;
	
	/// [RENDER TARGET]
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
	
	/// [SHADER]
	ID3D11PixelShader *m_pPixelShader = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;
	ID3D11VertexShader *m_pVertexShader = nullptr;

	/// [TEXTURE]
	ID3D11Texture2D *m_pRenderTargetTexture = nullptr;
	ID3D11ShaderResourceView* m_pSRV = nullptr;
};
