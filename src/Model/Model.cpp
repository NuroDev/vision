#include "Model.hpp"

HRESULT Model::Init()
{
    HRESULT hr = S_OK;

    for (Mesh* pMesh : m_pMeshes)
    {
        hr = pMesh->Init();
        if (FAILED(hr))
            return hr;
    }

    return hr;
}

void Model::Draw()
{
    for (Mesh* pMesh : m_pMeshes)
        pMesh->Draw();
}

void Model::Ui()
{
    if (!m_pMeshes.empty())
        if (ImGui::TreeNode("Meshes"))
        {
            const size_t numMeshes = m_pMeshes.size();
            for (unsigned int i = 0; i < numMeshes; ++i)
            {
                if (ImGui::TreeNode(std::to_string(i).c_str()))
                {
                    if (ImGui::TreeNode("Parameters"))
                    {
                        ImGui::Checkbox("Enabled", &m_pMeshes[i]->enabled);
                        ImGui::TreePop();
                    }

                    if (!m_pMeshes[i]->textures.empty())
                        if (ImGui::TreeNode("Textures"))
                        {
                            const size_t numTextures = m_pMeshes[i]->textures.size();
                            for (unsigned int j = 1; j < numTextures; ++j)
                            {
                                ImGui::Image(static_cast<void*>(m_pMeshes[j]->textures[j - 1]->data), ImVec2(96, 96));
                                if (j % 3)
                                    ImGui::SameLine();
                            }

                            ImGui::NewLine();
                            ImGui::TreePop();
                        }

                    if (ImGui::TreeNode("Material"))
                    {
                        ImGui::ColorEdit4("Ambient", reinterpret_cast<float*>(&m_pMeshes[i]->material.Ambient));
                        ImGui::ColorEdit4("Diffuse", reinterpret_cast<float*>(&m_pMeshes[i]->material.Diffuse));
                        ImGui::ColorEdit4("Emissive", reinterpret_cast<float*>(&m_pMeshes[i]->material.Emissive));
                        ImGui::ColorEdit4("Specular", reinterpret_cast<float*>(&m_pMeshes[i]->material.Specular));

                        ImGui::SliderFloat("Specular Power", &m_pMeshes[i]->material.SpecularPower, 0.0f, 128.0f);


                        ImGui::Checkbox("Texture", reinterpret_cast<bool*>(&m_pMeshes[i]->material.UseTexture));
                        ImGui::Checkbox("Normal Map", reinterpret_cast<bool*>(&m_pMeshes[i]->material.UseNormalMap));
                        ImGui::Checkbox("Parallax Mapping", reinterpret_cast<bool*>(&m_pMeshes[i]->material.UseParallaxMap));

                        if (m_pMeshes[i]->material.UseParallaxMap)
                        {
                            ImGui::Checkbox("Parallax Occlusion", reinterpret_cast<bool*>(&m_pMeshes[i]->material.UseParallaxOcclusion));

                            ImGui::InputFloat("Height", &m_pMeshes[i]->material.ParallaxHeight, 0.01);

                            if (ImGui::InputInt("Min Samples", &m_pMeshes[i]->material.ParallaxMinSamples, 1, 10))
                                if (m_pMeshes[i]->material.ParallaxMinSamples <= 1)
                                    m_pMeshes[i]->material.ParallaxMinSamples = 1;

                            if (ImGui::InputInt("Max Samples", &m_pMeshes[i]->material.ParallaxMaxSamples, 1, 10))
                                if (m_pMeshes[i]->material.ParallaxMaxSamples <= 1)
                                    m_pMeshes[i]->material.ParallaxMaxSamples = 1;
                        }

                        ImGui::TreePop();
                    }

                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
}

void Model::Update(float t)
{

}

void Model::Release()
{
    for (Mesh* pMesh : m_pMeshes)
        SAFE_RELEASE(pMesh);

    m_importer.FreeScene();
}

HRESULT Model::Load(const std::string path, unsigned flags)
{
    HRESULT hr = S_OK;

    // Create a new assimp instance and load the model
    const aiScene* pScene = m_importer.ReadFile(path, flags | aiProcess_ConvertToLeftHanded);

    // Check if the load from assimp was successful or not
    if (!pScene || pScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
        return E_FAIL;

    // Save the model files directory path
    m_directory = path.substr(0, path.find_last_of('/'));

    // Handle the data loaded from file
    hr = LoadNode(pScene->mRootNode, pScene);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"[MODEL]\nFailed to handle data loaded from model file.", L"Error", MB_OK);
        return hr;
    }

    return hr;
}

HRESULT Model::LoadNode(const aiNode* pNode, const aiScene* pScene)
{
    HRESULT hr = S_OK;
	
    // Load each mesh located at the current node
    for (unsigned int i = 0; i < pNode->mNumMeshes; ++i)
    {
        hr = LoadMesh(pScene->mMeshes[pNode->mMeshes[i]], pScene);
        if (FAILED(hr))
            return hr;
    }

    // After we've loading all of the meshes, we then recursively load each of the child nodes
    for (unsigned int i = 0; i < pNode->mNumChildren; ++i)
    {
        hr = LoadNode(pNode->mChildren[i], pScene);
        if (FAILED(hr))
            return hr;
    }

    return hr;
}

HRESULT Model::LoadMesh(aiMesh* pMesh, const aiScene* pScene)
{
    HRESULT hr = S_OK;
	
    // Data to fill
    std::vector<SimpleVertex> vertices;
    std::vector<UINT> indices;
    std::vector<Texture*> textures;

    // Vertices
    for (unsigned int i = 0; i < pMesh->mNumVertices; i++)
    {
        SimpleVertex vertex;

        // Position
        vertex.Position = XMFLOAT3(pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z);

        // Normals
        if (pMesh->HasNormals())
            vertex.Normal = XMFLOAT3(pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z);

        // Texture Coordinates
        if (pMesh->mTextureCoords[0]) {
            vertex.TexCoord = XMFLOAT2(pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y);
        }
        else {
            vertex.TexCoord = XMFLOAT2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // Tangents & Bi-normals/Bi-tangents
    if (pMesh->HasTangentsAndBitangents())
    {
        SimpleVertex vertex;

        vertex.Tangent = XMFLOAT3(pMesh->mTangents->x, pMesh->mTangents->y, pMesh->mTangents->z);
        vertex.BiNormal = XMFLOAT3(pMesh->mBitangents->x, pMesh->mBitangents->y, pMesh->mBitangents->z);

        vertices.push_back(vertex);
    }

    // Indices
    for (unsigned int i = 0; i < pMesh->mNumFaces; ++i)
        for (unsigned int j = 0; j < pMesh->mFaces[i].mNumIndices; ++j)
            indices.push_back(pMesh->mFaces[i].mIndices[j]);

    // TODO: Bones
    //if (pMesh->HasBones())
    //{
    //	for (unsigned int i = 0; i < pMesh->mNumBones; ++i)
    //	{
    //		
    //	}
    //}

    // Materials
    if (pMesh->mMaterialIndex >= 0)
    {
        const aiMaterial* pMaterial = pScene->mMaterials[pMesh->mMaterialIndex];

        // Diffuse maps
        std::vector<Texture*> diffuseMaps = LoadMaterialTextures(pScene, pMaterial, aiTextureType_DIFFUSE, "texture_diffuse").unwrap(); // TODO: Safe unwrap
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        // Specular maps
        std::vector<Texture*> specularMaps = LoadMaterialTextures(pScene, pMaterial, aiTextureType_SPECULAR, "texture_specular").unwrap(); // TODO: Safe unwrap
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    // Push back the newly loaded mesh
    m_pMeshes.push_back(new Mesh(vertices, indices, textures));

    return hr;
}

Result<std::vector<Texture*>, HRESULT> Model::LoadMaterialTextures(const aiScene* pScene, const aiMaterial* pMaterial, aiTextureType type, const std::string typeName)
{
    // Create new textures vector to return
    std::vector<Texture*> textures;

    for (unsigned int i = 0; i < pMaterial->GetTextureCount(type); i++)
    {
        aiString str;
        pMaterial->GetTexture(type, i, &str);

        // Attempt to load the texture from file
        Result<Texture*, HRESULT> textureResult = Texture::Load(str, typeName, pScene, Texture::GetIndex(&str));
        if (textureResult.isErr())
            return Err(E_FAIL); // TODO: Add error catch/return

        // Store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures
        textures.push_back(textureResult.unwrap()); // TODO: Safe unwrap
    }

    return Ok(textures);
}
