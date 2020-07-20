#include "PostProcessing.hpp"

HRESULT PostProcessing::Init()
{
	HRESULT hr = S_OK;

	// Get the graphics instance
	Graphics& graphics = Graphics::Get();

	// Check if the shaders have been loaded
	if (m_pPixelShader == nullptr || m_pVertexShader == nullptr)
	{
		MessageBox(nullptr, L"Post-processing shaders have not been initialized. Please load a shader before initializing the renderer.", L"Error", MB_OK);
		hr = E_FAIL;
	}

	// Initialize render to texture target description
	D3D11_TEXTURE2D_DESC textureDesc = {};
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = static_cast<UINT>(m_effects.ScreenWidth);
	textureDesc.Height = static_cast<UINT>(m_effects.ScreenHeight);
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	hr = graphics.GetDevice()->CreateTexture2D(&textureDesc, nullptr, &m_pRenderTargetTexture);
	if (FAILED(hr))
		return hr;

	// Initialize the render to texture target view description
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
	ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	hr = graphics.GetDevice()->CreateRenderTargetView(m_pRenderTargetTexture, &renderTargetViewDesc, &m_pRenderTargetView);
	if (FAILED(hr))
		return hr;

	// Initialize the render to texture shader resource view description
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	hr = graphics.GetDevice()->CreateShaderResourceView(m_pRenderTargetTexture, &shaderResourceViewDesc, &m_pSRV);
	if (FAILED(hr))
		return hr;

	// Initialize the quad
	m_pQuad = std::make_unique<PrimitiveBatch<VertexPositionTexture>>(graphics.GetContext());
	m_vertices = {	
		{XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)},
		{XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)},
		{XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f)},
	};
	m_indices = {
		0, 1, 2,
		3, 2, 1
	};

	// Create the effects constant buffer
	D3D11_BUFFER_DESC bd = {};
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(EffectsBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = graphics.GetDevice()->CreateBuffer(&bd, nullptr, &m_pEffectsConstantBuffer);
	if (FAILED(hr))
		return hr;

	return hr;
}

void PostProcessing::Draw(const std::function<void()> render)
{
	// Get the graphics instance
	Graphics& graphics = Graphics::Get();
	
	// Create light constant buffer
	EffectsBuffer effects;
	effects.Effects = m_effects;

	// Update light constant buffer using created light instance
	graphics.GetContext()->UpdateSubresource(m_pEffectsConstantBuffer, 0, nullptr, &effects, 0, 0);

	// Set effects constant buffer
	graphics.GetContext()->PSSetConstantBuffers(3, 1, &m_pEffectsConstantBuffer);
	
	// Set the render target back to the secondary render target (Off screen)
	SwapRenderTarget(m_pRenderTargetView, graphics.GetDepthStencilView(), graphics.GetBackgroundColor());

	// Draw content to the off screen target
	render();

	// Swap the render target back to the primary render target (Display)
	ID3D11RenderTargetView *pRenderTargetView = graphics.GetRenderTargetView();
	SwapRenderTarget(pRenderTargetView, graphics.GetDepthStencilView(), graphics.GetBackgroundColor());

	// Draw content to the primary target
	render();

	// Set shader
	graphics.GetContext()->VSSetShader(m_pVertexShader, nullptr, 0);
	graphics.GetContext()->PSSetShader(m_pPixelShader, nullptr, 0);
		
	graphics.GetContext()->IASetInputLayout(m_pInputLayout);

	m_pQuad->Begin();
	
	// Pass resources to shader (RTT texture)
	graphics.GetContext()->PSSetShaderResources(0, 1, &m_pSRV);

	// Draw quad to screen
	m_pQuad->DrawIndexed(
		graphics.GetParams()->doWireframe ? D3D11_PRIMITIVE_TOPOLOGY_LINELIST : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		m_indices.data(), 
		6, 
		m_vertices.data(),
		m_vertices.size()
	);
	
	m_pQuad->End();

	// Fix: Clear the shader resource after every frame
	ID3D11ShaderResourceView *const pShaderResourceView[1] = {nullptr};
	graphics.GetContext()->PSSetShaderResources(0, 1, pShaderResourceView);
}

void PostProcessing::Ui()
{
	if (ImGui::CollapsingHeader("Post-Processing"))
	{
		if (ImGui::TreeNode("Blur"))
		{
			ImGui::Checkbox("Enabled", reinterpret_cast<bool*>(&m_effects.Blur));

			if (m_effects.Blur)
			{
				ImGui::SameLine();
				ImGui::Checkbox("Gaussian Blur", reinterpret_cast<bool*>(&m_effects.Blur_Gaussian));
			}

			ImGui::SliderInt("Samples", &m_effects.Blur_Samples, 2, 100);
			ImGui::SliderFloat("Size", &m_effects.Blur_Size, 0.0f, 1.0f);

			if (m_effects.Blur_Gaussian)
				ImGui::SliderFloat("Standard Deviation", &m_effects.Blur_StandardDeviation, 0, 10);
			
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Color Space"))
		{
			enum ColorSpaces { RGB, HSV, YUV, CMY, NUM_SPACES };			
			const char* colorSpaceNames[NUM_SPACES] = { "RGB", "HSV", "YUV", "CMY" };			
			static int activeColorSpace = RGB;			
			const char* activeColorSpaceName = (activeColorSpace >= 0 && activeColorSpace < NUM_SPACES) ? colorSpaceNames[activeColorSpace] : "Unknown";
			
			if (ImGui::SliderInt("", &activeColorSpace, 0, NUM_SPACES - 1, activeColorSpaceName))
				m_effects.ColorSpace = activeColorSpace;

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Color Tint"))
		{
			ImGui::Checkbox("Enabled", reinterpret_cast<bool*>(&m_effects.ColorTint));
			ImGui::Checkbox("GameBoy", reinterpret_cast<bool*>(&m_effects.ColorTint_GameBoy));
			ImGui::Checkbox("Monochrome", reinterpret_cast<bool*>(&m_effects.ColorTint_GrayScale));
			ImGui::Checkbox("Invert", reinterpret_cast<bool*>(&m_effects.ColorTint_Invert));
			ImGui::Checkbox("Sepia", reinterpret_cast<bool*>(&m_effects.ColorTint_Sepia));
			ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&m_effects.ColorTint_Color));
			ImGui::TreePop();
		}
		
		if (ImGui::TreeNode("Parameters"))
		{
			ImGui::Checkbox("Picture in Picture", reinterpret_cast<bool*>(&m_effects.PictureInPicture));			
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Pixelation"))
		{
			ImGui::Checkbox("Enabled", reinterpret_cast<bool*>(&m_effects.Pixelate));
			ImGui::SliderFloat("Factor", &m_effects.PixelationFactor, 0.1f, 25.0f);			
			ImGui::TreePop();
		}
		
		if (ImGui::TreeNode("Render-To-Texture"))
		{
			ImGui::Image(static_cast<void*>(m_pSRV), ImVec2(288, 160));
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Vignette"))
		{
			ImGui::Checkbox("Enabled", reinterpret_cast<bool*>(&m_effects.Vignette));

			ImGui::SliderFloat("Amount", &m_effects.VignetteAmount, -10.0f, 10.0f);
			ImGui::SliderFloat("Radius", &m_effects.VignetteRadius, 0.0f, 5.0f);
			ImGui::SliderFloat("Ratio", &m_effects.VignetteRatio, 0.0f, 5.0f);
			ImGui::SliderInt("Slope", &m_effects.VignetteSlope, 0, 25);

			ImGui::TreePop();
		}
	}
}

void PostProcessing::Release()
{
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pSRV);

	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexShader);

	SAFE_RELEASE(m_pRenderTargetTexture);
}

void PostProcessing::SwapRenderTarget(ID3D11RenderTargetView *pRenderTargetView, ID3D11DepthStencilView *pDepthStencilView, const FLOAT colors[4])
{
	// Get the graphics instance
	Graphics& graphics = Graphics::Get();

	// Set the render target back to the back buffer
	graphics.GetContext()->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);

	// Clear the back buffer
	graphics.GetContext()->ClearRenderTargetView(pRenderTargetView, colors);

	// Clear the depth buffer
	graphics.GetContext()->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

HRESULT PostProcessing::LoadShader(const WCHAR *path, LPCSTR vertexEntry, LPCSTR pixelEntry)
{
	HRESULT hr;
	ID3DBlob *pVSBlob = nullptr;
	ID3DBlob *pPSBlob = nullptr;

	// Get graphics instance
	Graphics& graphics = Graphics::Get();

	// Load vertex shader from post processing shader file
	hr = Helpers::CompileShaderFromFile(path, vertexEntry, "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = graphics.GetDevice()->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_pVertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// Load pixel shader from post processing shader file
	hr = Helpers::CompileShaderFromFile(path, pixelEntry, "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = graphics.GetDevice()->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pPixelShader);
	if (FAILED(hr))
	{
		pPSBlob->Release();
		return hr;
	}

	// Create new input layout
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT numElements = ARRAYSIZE(layout);

	hr = graphics.GetDevice()->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pInputLayout);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	return hr;
}
