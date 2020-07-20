#pragma once

#include <DirectXMath.h>
#include <memory>

using namespace DirectX;

struct ConstantBufferData
{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;
	XMFLOAT4 OutputColor;
};

class ConstantBuffer
{
public:
	static ConstantBuffer& Get()
	{
		if (m_pBuffer == nullptr)
			m_pBuffer = std::unique_ptr<ConstantBuffer>(new ConstantBuffer());

		return *m_pBuffer;
	};

	/// [MATRIX] Get the constant buffer matrix
	ConstantBufferData GetData() const { return m_matrix; };

	/// [MATRIX] Get the constant buffer world matrix
	XMMATRIX GetWorld() const { return m_matrix.World; };

	/// [MATRIX] Get the constant buffer view matrix
	XMMATRIX GetView() const { return m_matrix.View; };

	/// [MATRIX] Get the constant buffer projection matrix
	XMMATRIX GetProjection() const { return m_matrix.World; };

	/// [MATRIX] Get the constant buffer output color
	XMFLOAT4 GetColor() const { return m_matrix.OutputColor; };

	/// [MATRIX] Set the constant buffer data
	void SetData(const ConstantBufferData matrix) { m_matrix = matrix; };

	/// [MATRIX] Set the constant buffer world matrix
	void SetWorld(const XMMATRIX matrix) { m_matrix.World = matrix; };

	/// [MATRIX] Set the constant buffer view matrix
	void SetView(const XMMATRIX matrix) { m_matrix.View = matrix; };

	/// [MATRIX] Set the constant buffer projection matrix
	void SetProjection(const XMMATRIX matrix) { m_matrix.Projection = matrix; };

	/// [MATRIX] Set the constant buffer output color
	void SetColor(const XMFLOAT4 color) { m_matrix.OutputColor = color; };

private:
	ConstantBuffer() = default;
	ConstantBuffer(const ConstantBuffer&);
	ConstantBuffer& operator= (const ConstantBuffer&);
	
	/// [INSTANCE]
	static std::unique_ptr<ConstantBuffer> m_pBuffer;

	/// [MATRIX]
	ConstantBufferData m_matrix;
};
