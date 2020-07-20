#pragma once

#include <cstring>
#include <DirectXMath.h>

using namespace DirectX;

struct SimpleVertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
	XMFLOAT3 Tangent;
	XMFLOAT3 BiNormal;

	bool operator<(const SimpleVertex other) const { return memcmp((void*)this, (void*)&other, sizeof(SimpleVertex)) > 0; }
};

struct SimpleQuad
{
	XMFLOAT3 Position;
	XMFLOAT2 TexCoord;
};
