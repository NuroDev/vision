#pragma once

#include "../Graphics.hpp"

class DeferredShader
{
public:
	~DeferredShader() { Release(); };

	/// [DEFERRED SHADER] Initialize any core renderer data
	HRESULT Init();

	/// [DEFERRED SHADER] Draw the contents of the shader to the screen
	void Draw(const int numIndices, ID3D11ShaderResourceView* pSRV);

	/// [DEFERRED SHADER] Cleanup & release the object when it is done
	void Release();

	/// [DEFERRED SHADER] Load a shader from a specified path
	HRESULT LoadShader(const WCHAR* path, const char* vsEntry = "VS", const char* psEntry = "PS");
private:
	/// [DEFERRED SHADER] Set the shader parameters
	HRESULT SetShaderParameters(ID3D11ShaderResourceView* pSRV);

	/// [BUFFER]
	ID3D11Buffer* m_pMatrixBuffer = nullptr;
	
	/// [SAMPLER]
	ID3D11SamplerState* m_pSampleState = nullptr;
	
	/// [SHADER]
	ID3D11VertexShader* m_pVertexShader = nullptr;
	ID3D11PixelShader* m_pPixelShader = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;
};
