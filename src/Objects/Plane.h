#pragma once

#include "WorldObject.h"

class Plane : public WorldObject
{
public:
	Plane (
		std::string name,
		const unsigned int id,
		Transform* pTransform = new Transform()
	) : WorldObject(name, id, pTransform) {};
	~Plane() { WorldObject::Release(); };

	/// [PLANE] Initialize any core object data
	HRESULT Init()
	{
		HRESULT hr = S_OK;

		// Define static mesh data
		std::vector<SimpleVertex> vertices = {
			{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},

			{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},

			{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},

			{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},

			{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},

			{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
		};
		const std::vector<UINT> indices = {
			19, 17, 16,
			18, 17, 19,
		};

		// Calculate normals, tangents & bi-normals/bi-tangents for static mesh data
		CalculateModelVectors(vertices, static_cast<const unsigned int>(vertices.size()));

		// Create a new mesh instance if one doesn't already exist
		// (EG: Textures have been provided, thus pre-initializing one)
		Mesh* pMesh = GetMesh().empty() ? new Mesh(vertices, indices) : GetMesh(0);

		// Set the world object mesh
		SetMesh({ pMesh });

		// Initialize the object	
		hr = WorldObject::Init();
		if (FAILED(hr))
			return hr;

		return hr;
	};

private:
	/// [PLANE] Calculate the tangent & normal vectors for every face of the cube
	void CalculateModelVectors(std::vector<SimpleVertex>& vertices, const unsigned int vertexCount)
	{
		SimpleVertex v1, v2, v3;
		XMFLOAT3 tangents, normals;

		// Calculate the number of faces in the model
		const int faceCount = vertexCount / 3;

		// Initialize the index to the model data
		int index = 0;

		// Go through all the faces and calculate the tangent & normal vectors
		for (int i = 0; i < faceCount; i++)
		{
			// TODO: Is this needed?
			//if (i == 8)
			//	int x = 1;

			// Get the three vertices for this face from the model
			v1.Position = vertices[index].Position;
			v1.TexCoord = vertices[index].TexCoord;
			v1.Normal = vertices[index].Normal;
			index++;

			v2.Position = vertices[index].Position;
			v2.TexCoord = vertices[index].TexCoord;
			v2.Normal = vertices[index].Normal;
			index++;

			v3.Position = vertices[index].Position;
			v3.TexCoord = vertices[index].TexCoord;
			v3.Normal = vertices[index].Normal;
			index++;


			CalculateTangentBiNormal(v1, v2, v3, normals, tangents);

			// Store the normal & tangents for this face back in the model structure
			vertices[index - 1].Normal = normals;
			vertices[index - 1].Tangent = tangents;

			vertices[index - 2].Normal = normals;
			vertices[index - 2].Tangent = tangents;

			vertices[index - 3].Normal = normals;
			vertices[index - 3].Tangent = tangents;
		}
	};

	/// [PLANE] TODO: Documentation
	void CalculateTangentBiNormal(SimpleVertex v0, SimpleVertex v1, SimpleVertex v2, XMFLOAT3& normals, XMFLOAT3& tangents)
	{
		XMFLOAT3 edge1(v1.Position.x - v0.Position.x, v1.Position.y - v0.Position.y, v1.Position.z - v0.Position.z);
		XMFLOAT3 edge2(v2.Position.x - v0.Position.x, v2.Position.y - v0.Position.y, v2.Position.z - v0.Position.z);

		XMFLOAT2 deltaUV1(v1.TexCoord.x - v0.TexCoord.x, v1.TexCoord.y - v0.TexCoord.y);
		XMFLOAT2 deltaUV2(v2.TexCoord.x - v0.TexCoord.x, v2.TexCoord.y - v0.TexCoord.y);

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangents.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangents.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangents.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

		XMVECTOR tn = XMLoadFloat3(&tangents);
		tn = XMVector3Normalize(tn);
		XMStoreFloat3(&tangents, tn);

		XMVECTOR vv0 = XMLoadFloat3(&v0.Position);
		XMVECTOR vv1 = XMLoadFloat3(&v1.Position);
		XMVECTOR vv2 = XMLoadFloat3(&v2.Position);

		XMVECTOR e0 = vv1 - vv0;
		XMVECTOR e1 = vv2 - vv0;

		XMVECTOR e01cross = XMVector3Cross(e0, e1);
		e01cross = XMVector3Normalize(e01cross);
		XMFLOAT3 normalOut;
		XMStoreFloat3(&normalOut, e01cross);
		normals = normalOut;
	};
};
