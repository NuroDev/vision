#pragma once

#include <vector>

#include "../Graphics/Graphics.h"
#include "Texture.h"

class Mesh
{
public:
    Mesh() = default;
	
    Mesh(
        const std::vector<SimpleVertex> vertices = {},
        const std::vector<UINT> indices = {},
        const std::vector<Texture*> pTextures = {}
    ) : vertices(vertices),
        indices(indices),
        textures(pTextures)
    {};
	
    ~Mesh() { Release(); };

    /// [MESH] Initialize the core mesh data
    HRESULT Init()
    {
        HRESULT hr = S_OK;

        if (vertices.empty())
        {
            MessageBox(nullptr, L"[MESH]\nMesh vertices array is empty, please make sure that vertices are being loaded correctly.", L"Error", MB_OK);
            return E_FAIL;
        }

        if (indices.empty())
        {
            MessageBox(nullptr, L"[MESH]\nMesh indices array is empty, please make sure that indices are being loaded correctly.", L"Error", MB_OK);
            return E_FAIL;
        }

        // Get the graphics instance
        Graphics& graphics = Graphics::Get();

        // Create empty buffer description
        D3D11_BUFFER_DESC bd = {};

        // Create empty sub-resource data instance
        D3D11_SUBRESOURCE_DATA InitData = {};

        // Initialize vertex buffer
        ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = static_cast<UINT>(sizeof(SimpleVertex) * vertices.size());
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
        InitData.pSysMem = static_cast<const void*>(vertices.data());
        hr = graphics.GetDevice()->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);
        if (FAILED(hr))
            return hr;

        // Initialize index buffer
        ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        InitData.pSysMem = static_cast<const void*>(indices.data());
        hr = graphics.GetDevice()->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);
        if (FAILED(hr))
            return hr;

        return hr;
    };

    /// [MESH] Draw the mesh to the screen
    void Draw()
    {
        // Check if the object is enabled
        if (!enabled)
            return;
    	
        // Get the graphics instance
        Graphics& graphics = Graphics::Get();

        // Set material constant buffer
        graphics.GetContext()->UpdateSubresource(graphics.GetMaterialBuffer(), 0, nullptr, &material, 0, 0);

        // Set the buffers
        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        graphics.GetContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
        graphics.GetContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

        // Pass the textures to the shader
        const size_t numTextures = textures.size();
        for (unsigned int i = 0; i < numTextures; ++i)
            graphics.GetContext()->PSSetShaderResources(i, 1, &textures[i]->data);

        // Set the topology
        graphics.GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Draw indices
        if (!graphics.GetParams()->doDeferred)
            graphics.GetContext()->DrawIndexed(static_cast<UINT>(indices.size()), 0, 0);
    };

    /// [MESH] Cleanup & release the mesh when it's done
    void Release()
    {
        SAFE_RELEASE(m_pVertexBuffer);
        SAFE_RELEASE(m_pIndexBuffer);
        for (Texture* pTexture : textures)
            SAFE_RELEASE(pTexture);
    };

    /// [DATA]
    std::vector<SimpleVertex> vertices;
    std::vector<UINT> indices;
    std::vector<Texture*> textures;
    Material material;
    bool enabled = true;
	
private:
    /// [BUFFER]
    ID3D11Buffer *m_pVertexBuffer = nullptr;
    ID3D11Buffer *m_pIndexBuffer = nullptr;
};
