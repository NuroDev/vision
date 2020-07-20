#pragma once

#include <DirectXMesh.h>
#include <GeometricPrimitive.h>

#include "WorldObject.hpp"

class Sphere : public WorldObject
{
public:
	Sphere(
		std::string name, 
		const unsigned int id,
		Transform* pTransform = new Transform()
	) : WorldObject(name, id, pTransform) {};
	~Sphere() { WorldObject::Release(); };

	/// [SPHERE] Initialize any core object data
	HRESULT Init()
	{
		HRESULT hr = S_OK;
		
		// Create vectors to store the initial primitive data
		std::vector<VertexPositionNormalTexture> primitiveVertices;
		std::vector<uint16_t> primitiveInidices;

		GeometricPrimitive::CreateSphere(primitiveVertices, primitiveInidices, GetTransform()->GetScale().x, 16, false);

		// Convert from geometric primitive vectors to custom mesh data
		const size_t numVertices = primitiveVertices.size();
		std::vector<SimpleVertex> vertices(numVertices);
		for (unsigned int i = 0; i < numVertices; ++i)
		{
			vertices[i].Position = primitiveVertices[i].position;
			vertices[i].Normal = primitiveVertices[i].normal;
			vertices[i].TexCoord= primitiveVertices[i].textureCoordinate;
		}
		const size_t numIndices = primitiveInidices.size();
		std::vector<UINT> indices(numIndices);
		for (unsigned int i = 0; i < numIndices; ++i)
			indices[i] = primitiveInidices[i];

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

	/// [SPHERE] Update the object every tick
	void Update(float t)
	{
		// Get world object transform
		Transform* pTransform = GetTransform();

		// Reset transform
		if (GetAsyncKeyState('R'))
		{
			pTransform->SetPosition(Defaults::Transform::Position);
			pTransform->SetRotation(Defaults::Transform::Rotation);
		}

		// Hand over to the standard world object update
		WorldObject::Update(t);
	};
};
