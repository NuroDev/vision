#pragma once

#include "../Graphics.hpp"

class LightShader
{
public:
	~LightShader() { Release(); };

	/// [LIGHT SHADER] Initialize any core renderer data
	HRESULT Init();

	/// [LIGHT SHADER] Draw the contents of the shader to the screen
	void Draw(const int numIndices, ID3D11ShaderResourceView* pColorSRV, ID3D11ShaderResourceView* pNormalSRV, XMFLOAT4 lightDirection);

	/// [LIGHT SHADER] Cleanup & release the object when it is done
	void Release();

	/// [LIGHT SHADER] Load a shader from a specified path
	HRESULT LoadShader(const WCHAR* path, const char* vsEntry = "VS", const char* psEntry = "PS");
private:
	/// [LIGHT SHADER] Set the shader parameters
	HRESULT SetShaderParameters(ID3D11ShaderResourceView* pColorSRV, ID3D11ShaderResourceView* pNormalSRV, XMFLOAT4 lightDirection);
	
	/// [BUFFER]
	ID3D11Buffer* m_pMatrixBuffer = nullptr;
	ID3D11Buffer* m_pLightBuffer = nullptr;
	
	/// [SHADER]
	ID3D11VertexShader* m_pVertexShader = nullptr;
	ID3D11PixelShader* m_pPixelShader = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;

	/// [SAMPLER]
	ID3D11SamplerState* m_pSampleState = nullptr;
};
