#pragma once

#include <cstring>
#include <DirectXMath.h>

// TODO: Make default constant
#define NUM_BONES_PER_VERTEX 4

struct SimpleVertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
	XMFLOAT3 Tangent;
	XMFLOAT3 BiNormal;

	bool operator<(const SimpleVertex other) const { return memcmp((void*)this, (void*)&other, sizeof(SimpleVertex)) > 0; }
};

struct SkinnedVertex : public SimpleVertex
{
	int ids[NUM_BONES_PER_VERTEX];
	float weights[NUM_BONES_PER_VERTEX];

	SkinnedVertex(
		const XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f),
		const XMFLOAT3 normal = XMFLOAT3(0.0f, 0.0f, 0.0f),
		const XMFLOAT2 texCoord = XMFLOAT2(0.0f, 0.0f),
		const XMFLOAT3 tangent = XMFLOAT3(0.0f, 0.0f, 0.0f),
		const XMFLOAT3 biNormal = XMFLOAT3(0.0f, 0.0f, 0.0f)
	) {
		this->Position = position;
		this->Normal = normal;
		this->TexCoord = texCoord;
		this->Tangent = tangent;
		this->BiNormal = biNormal;
		
		memset(ids, 0, sizeof(ids));
		memset(weights, 0, sizeof(weights));
	};

	SkinnedVertex(const SimpleVertex vertex)
	{
		this->Position = vertex.Position;
		this->Normal = vertex.Normal;
		this->TexCoord = vertex.TexCoord;
		this->Tangent = vertex.Tangent;
		this->BiNormal = vertex.BiNormal;
		
		memset(ids, 0, sizeof(ids));
		memset(weights, 0, sizeof(weights));
	};

	bool operator<(const SimpleVertex other) const { return memcmp((void*)this, (void*)&other, sizeof(SimpleVertex)) > 0; }
};

struct SimpleQuad
{
	XMFLOAT3 Position;
	XMFLOAT2 TexCoord;
};
