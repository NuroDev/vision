#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Config/Defaults.h"
#include "Mesh.h"

class Model
{
public:
	~Model() { Release(); };

	/// [MODEL] Initialize any core model data
	HRESULT Init();

	/// [MODEL] Draw all the models meshes
	void Draw();

	/// [MODEL] Draw the custom imgui ui to the screen
	void Ui();

	/// [MODEL] Update the model every tick
	void Update(float t);

	/// [MODEL] Cleanup & release the model when it is done
	void Release();
	
	/// [MODEL] Loads a model from a specifIed file
    HRESULT Load(const std::string path, unsigned int flags = Defaults::Model::Flags);

	/// [MESH] Get the meshes vector
	std::vector<Mesh*> GetMesh() const { return m_pMeshes; };

	/// [MESH] Get a mesh from a specified index
	Mesh* GetMesh(const unsigned int index) const { return m_pMeshes[index]; };

	/// [MESH] Set the meshes vector
	void SetMesh(std::vector<Mesh*> pMeshes) { m_pMeshes = std::move(pMeshes); };

	/// [MESH] Set a mesh at a specified index
	void SetMesh(Mesh* pMesh, const unsigned int index) { m_pMeshes[index] = pMesh; };
	
private:
	/// [MODEL] Recursively load an assimp node and all its children
    HRESULT LoadNode(const aiNode* pNode, const aiScene* pScene);
	
	/// [MODEL] Load a single mesh from an assimp mesh
    HRESULT LoadMesh(aiMesh* pMesh, const aiScene* pScene);

    // [MODEL] Iterates through a provided material and load all textures from it
    Result<std::vector<Texture*>, HRESULT> LoadMaterialTextures(const aiScene* pScene, const aiMaterial* pMaterial, aiTextureType type, const std::string typeName);

	std::vector<Mesh*> m_pMeshes;

	std::string m_directory;

	Assimp::Importer m_importer;
};
