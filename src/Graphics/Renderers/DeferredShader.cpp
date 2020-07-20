#include "DeferredShader.hpp"

HRESULT DeferredShader::Init()
{
	HRESULT hr = S_OK;

	if (m_pPixelShader == nullptr || m_pInputLayout == nullptr || m_pVertexShader == nullptr)
	{
		MessageBox(nullptr, L"[DEFERRED SHADER]\nNo shader has been initialized. Please make sure a shader has been loaded before initializing.", L"Error", MB_OK);
		return hr;
	}
	
	return hr;
}

void DeferredShader::Draw(const int numIndices, ID3D11ShaderResourceView* pSRV)
{
	// Get graphics instance
	Graphics& graphics = Graphics::Get();

	// Set the shader parameters
	if (FAILED(SetShaderParameters(pSRV)))
	{
		MessageBox(nullptr, L"[DEFERRED SHADER]\nFailed to set shader parameters.", L"Error", MB_OK);
		return;
	}
	
	// Set the vertex input layout
	graphics.GetContext()->IASetInputLayout(m_pInputLayout);

	// Set the vertex & pixel shaders
	graphics.GetContext()->VSSetShader(m_pVertexShader, NULL, 0);
	graphics.GetContext()->PSSetShader(m_pPixelShader, NULL, 0);

	// Set the sampler states in the pixel shader
	graphics.GetContext()->PSSetSamplers(0, 1, &m_pSampleState);

	// Render the geometry
	graphics.GetContext()->DrawIndexed(numIndices, 0, 0);
}

void DeferredShader::Release()
{
	SAFE_RELEASE(m_pMatrixBuffer);
	
	SAFE_RELEASE(m_pSampleState);

	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pInputLayout);
}

HRESULT DeferredShader::LoadShader(const WCHAR* path, const char* vsEntry, const char* psEntry)
{
	HRESULT hr = S_OK;

	// Get graphics instance
	Graphics& graphics = Graphics::Get();

	// Create & compile a new vertex shader
	ID3DBlob* pVSBlob = nullptr;
	hr = Helpers::CompileShaderFromFile(path, vsEntry, "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"The shader file's vertex entry cannot be compiled. Please validate the shader code.", L"Error", MB_OK);
		return hr;
	}
	hr = graphics.GetDevice()->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_pVertexShader);
	if (FAILED(hr))
		return hr;

	// Create & compile a new pixel shader
	ID3DBlob* pPSBlob = nullptr;
	hr = Helpers::CompileShaderFromFile(path, psEntry, "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"The shader file's pixel entry cannot be compiled. Please validate the shader code.", L"Error", MB_OK);
		return hr;
	}
	hr = graphics.GetDevice()->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pPixelShader);
	if (FAILED(hr))
		return hr;

	// Define & create a new input layout
	D3D11_INPUT_ELEMENT_DESC layout [3] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	hr = graphics.GetDevice()->CreateInputLayout(layout, ARRAYSIZE(layout), pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pInputLayout);
	if (FAILED(hr))
		return hr;
	graphics.GetContext()->IASetInputLayout(m_pInputLayout);

	// Cleanup blobs
	SAFE_RELEASE(pVSBlob);
	SAFE_RELEASE(pVSBlob);

	// Create a wrap texture sampler state
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = graphics.GetDevice()->CreateSamplerState(&samplerDesc, &m_pSampleState);
	if (FAILED(hr))
		return hr;

	// Initialize the dynamic matrix constant buffer that is in the vertex shader
	D3D11_BUFFER_DESC matrixBufferDesc = {};
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(ConstantBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	hr = graphics.GetDevice()->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer);
	if (FAILED(hr))
		return hr;
	
	return hr;
}

HRESULT DeferredShader::SetShaderParameters(ID3D11ShaderResourceView* pSRV)
{
	HRESULT hr = S_OK;

	// Get graphics instance
	Graphics& graphics = Graphics::Get();
	
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};

	// Transpose the matrices to prepare them for the shader
	ConstantBuffer& constantBuffer = ConstantBuffer::Get();
	constantBuffer.SetWorld(XMMatrixTranspose(constantBuffer.GetWorld()));
	constantBuffer.SetView(XMMatrixTranspose(constantBuffer.GetView()));
	constantBuffer.SetProjection(XMMatrixTranspose(constantBuffer.GetProjection()));

	// Lock the constant buffer so it can be written to
	hr = graphics.GetContext()->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
		return hr;

	// Get a pointer to the data in the constant buffer
	ConstantBufferData* pNewConstantBuffer = static_cast<ConstantBufferData*>(mappedResource.pData);
	pNewConstantBuffer->World = constantBuffer.GetWorld();
	pNewConstantBuffer->View = constantBuffer.GetView();
	pNewConstantBuffer->Projection = constantBuffer.GetProjection();

	// Unlock the constant buffer.
	graphics.GetContext()->Unmap(m_pMatrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader
	unsigned int bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values
	graphics.GetContext()->VSSetConstantBuffers(bufferNumber, 1, &m_pMatrixBuffer);

	// Set shader texture resource in the pixel shader
	graphics.GetContext()->PSSetShaderResources(0, 1, &pSRV);

	return hr;
}
