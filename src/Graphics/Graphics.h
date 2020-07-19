#pragma once

#include <d3d11_1.h>
#include <DirectXColors.h>

#include "ConstantBuffer.h"
#include "Layouts.h"
#include "Window.h"
#include "../Camera/FirstPersonCamera.h"
#include "../Config/Structrures.h"
#include "../Model/Material.h"

using namespace DirectX;

class Graphics
{
public:
	~Graphics() { Release(); };

	/// [GRAPHICS] Initialize any core graphics data
	HRESULT Init();

	/// [GRAPHICS] Set any core graphics options before drawing content to the screen
	void Draw();

	/// [GRAPHICS] Draw the custom imgui UI to the screen
	void Ui();

	/// [GRAPHICS] Cleanup & release the graphics instance when done
	void Release();

	/// [BUFFER] Get the primary constant buffer
	ID3D11Buffer *GetConstantBuffer() const { return m_pConstantBuffer; };

	/// [BUFFER] Get the primary depth stencil buffer
	ID3D11Texture2D *GetDepthStencilBuffer() const { return m_pDepthStencilBuffer; };

	/// [BUFFER] Get the primary material buffer
	ID3D11Buffer* GetMaterialBuffer() const { return m_pMaterialConstantBuffer; };

	/// [BUFFER] Set the primary constant buffer
	void SetConstantBuffer(ID3D11Buffer *pBuffer) { m_pConstantBuffer = pBuffer; };

	/// [BUFFER] Set the primary stencil buffer
	void SetDepthStencilBuffer(ID3D11Texture2D *pBuffer) { m_pDepthStencilBuffer = pBuffer; };

	/// [BUFFER] Set the primary material buffer
	void SetMaterialBuffer(ID3D11Buffer* pBuffer) { m_pMaterialConstantBuffer = pBuffer; };

	/// [COLOR] Get the background color
	XMVECTORF32 GetBackgroundColor() const { return m_backgroundColor; };

	/// [COLOR] Set the background color
	void SetBackgroundColor(XMVECTORF32 color) { m_backgroundColor = color; };
	
	/// [DEVICE] Get the primary d3d device
	ID3D11Device *GetDevice() const { return m_pDevice; };

	/// [DEVICE] Get the secondary d3d device
	ID3D11Device *GetDeviceAlt() const { return m_pDeviceAlt; };

	/// [DEVICE] Get the primary d3d device context
	ID3D11DeviceContext *GetContext() const { return m_pContext; };

	/// [DEVICE] Get the secondary d3d device context
	ID3D11DeviceContext *GetContextAlt() const { return m_pContextAlt; };

	/// [DEVICE] Set the primary d3d device
	void SetDevice(ID3D11Device *device) { m_pDevice = device; };

	/// [DEVICE] Set the scondary d3d device
	void SetDeviceAlt(ID3D11Device *device) { m_pDeviceAlt = device; };

	/// [DEVICE] Set the primary d3d device context
	void SetContext(ID3D11DeviceContext *context) { m_pContext = context; };

	/// [DEVICE] Set the secondary d3d device context
	void SetContextAlt(ID3D11DeviceContext *context) { m_pContextAlt = context; };

	/// [INSTANCE] Get the graphics singleton instance
	static Graphics& Get();

	/// [PARAMS] Get the current parameters object
	GraphicsParameters* GetParams() const { return m_pParams; };

	/// [PARAMS] Set the parameters object
	void SetParams(GraphicsParameters* pParams) { m_pParams = pParams; };

	/// [PARAMS] Get the material
	Material GetMaterial() const { return m_material; };

	/// [PARAMS] Set the material
	void SetMaterial(Material material) { m_material = material; };

	/// [RASTERIZER] Get the current wireframe rendering rasterizer state
	ID3D11RasterizerState *GetWireFrame() const { return m_pWireFrame; };

	/// [RASTERIZER] Get the current solid renderer rasterizer state
	ID3D11RasterizerState *GetSolidObject() const { return m_pSolidObject; };

	/// [RASTERIZER] Set the current wireframe renderer rasterizer state
	void SetWireFrame(ID3D11RasterizerState *state) { m_pWireFrame = state; };

	/// [RASTERIZER] Set the current solid renderer rasterizer state
	void SetSolidObject(ID3D11RasterizerState *state) { m_pSolidObject = state; };

	/// [SAMPLER] Get the primary sampler state
	ID3D11SamplerState *GetSamplerLinear() const { return m_pSamplerLinear; };

	/// [SAMPLER] Set the primary sampler state
	void SetSamplerLinear(ID3D11SamplerState *sampler) { m_pSamplerLinear = sampler; };

	/// [SWAP CHAIN] Get the primary device swap chain
	IDXGISwapChain *GetSwapChain() const { return m_pSwapChain; };

	/// [SWAP CHAIN] Get the secondary device swap chain
	IDXGISwapChain1 *GetSwapChainAlt() const { return m_pSwapChainAlt; };

	/// [SWAP CHAIN] Set the primary device swap chain
	void SetSwapChain(IDXGISwapChain *swapchain) { m_pSwapChain = swapchain; };

	/// [SWAP CHAIN] Set the secondary device swap chain
	void SetSwapChainAlt(IDXGISwapChain1 *swapchain) { m_pSwapChainAlt = swapchain; };

	/// [VIEW] Get the device stencil view
	ID3D11DepthStencilView *GetDepthStencilView() const { return m_pDepthStencilView; };

	/// [VIEW] Get the primary device render target view
	ID3D11RenderTargetView *GetRenderTargetView() const { return m_pRenderTargetView; };

	/// [VIEW] Set the device stencil view
	void SetDepthStencilView(ID3D11DepthStencilView *view) { m_pDepthStencilView = view; };

	/// [VIEW] Set the primary device render target view
	void SetRenderTargetView(ID3D11RenderTargetView *view) { m_pRenderTargetView = view; };

	/// [VIEW] Swap to the back buffer render target
	void SwapBackBuffer() { m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView); };

	/// [VIEWPORT] Get the viewport
	D3D11_VIEWPORT GetViewport() const { return m_viewport; };

	/// [VIEWPORT] Set the viewport
	void SetViewport(D3D11_VIEWPORT viewport) { m_viewport = viewport; };

	/// [VIEWPORT] Reset the viewport
	void ResetViewport(UINT numViewports = 1) const { m_pContext->RSSetViewports(numViewports, &m_viewport); };
	
private:
	Graphics() {};
	Graphics(const Graphics &g);
	Graphics& operator= (const Graphics&);

	/// [GRAPHICS] Initialize the for system devices
	HRESULT InitDevice(HWND hWnd);

	/// [BUFFER]
	ID3D11Buffer *m_pConstantBuffer = nullptr;
	ID3D11Texture2D *m_pDepthStencilBuffer = nullptr;
	ID3D11Buffer* m_pMaterialConstantBuffer = nullptr;

	/// [COLOR]
	XMVECTORF32 m_backgroundColor = Colors::SkyBlue;

	/// [DEVICE]
	ID3D11Device *m_pDevice = nullptr;
	ID3D11Device *m_pDeviceAlt = nullptr;
	ID3D11DeviceContext *m_pContext = nullptr;
	ID3D11DeviceContext *m_pContextAlt = nullptr;

	/// [INSTANCE]
	static std::unique_ptr<Graphics> m_pInstance;

	/// [PARAMETERS]
	GraphicsParameters* m_pParams = nullptr;

	/// [MATERIAL]
	Material m_material;
	
	/// [MISC]
	D3D_DRIVER_TYPE m_driverType;
	D3D_FEATURE_LEVEL m_featureLevel;

	/// [RASTERIZER]
	ID3D11RasterizerState *m_pWireFrame = nullptr;
	ID3D11RasterizerState *m_pSolidObject = nullptr;
	ID3D11BlendState *m_pTransparency = nullptr;

	/// [SAMPLER]
	ID3D11SamplerState *m_pSamplerLinear = nullptr;

	/// [SWAP CHAIN]
	IDXGISwapChain *m_pSwapChain = nullptr;
	IDXGISwapChain1 *m_pSwapChainAlt = nullptr;

	/// [VIEW]
	ID3D11DepthStencilView *m_pDepthStencilView = nullptr;
	ID3D11RenderTargetView *m_pRenderTargetView = nullptr;

	/// [VIEWPORT]
	D3D11_VIEWPORT m_viewport;
};
