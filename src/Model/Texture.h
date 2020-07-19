#pragma once

#include <assimp/scene.h>
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <result.h>

enum class TextureType
{
	CUSTOM_PATH,
	TEXTURE_DIFFUSE,
	TEXTURE_SPECULAR,
	UNKNOWN,
};

struct Texture
{	
	~Texture() { Release(); };

	/// [TEXTURE] Cleanup & release the texture when done
	void Release()
	{
		SAFE_RELEASE(data);
	}

	/// [TEXTURE] Get a texture type from a string
	static TextureType GetType(const std::string type)
	{
		const std::string typeStr(type.data());
		
		if (typeStr.compare("texture_diffuse"))
			return TextureType::TEXTURE_DIFFUSE;
		if (typeStr.compare("texture_specular"))
			return TextureType::TEXTURE_SPECULAR;

		return TextureType::UNKNOWN;
	}

	/// [MODEL LOADER] Get the texture index from a provided assimp string
	static int GetIndex(aiString* pStr)
	{
		return atoi(static_cast<std::string>(pStr->C_Str()).substr(1).c_str());
	}
	
	/// [TEXTURE] Load a texture from a specified path & return a shader resource view of the texture data
	static Result<Texture*, HRESULT> Load(const aiString path, const std::string typeStr, const aiScene* pScene, const int index)
	{
		HRESULT hr = S_OK;

		Texture* texture = new Texture();
		texture->path = path;
		texture->type = GetType(typeStr.c_str());
		texture->data = nullptr;
		
		// Get the graphics instance
		Graphics& graphics = Graphics::Get();
		
		int* size = reinterpret_cast<int*>(&pScene->mTextures[index]->mWidth);
		
		hr = CreateWICTextureFromMemory(graphics.GetDevice(), graphics.GetContext(), reinterpret_cast<unsigned char*>(pScene->mTextures[index]->pcData), *size, nullptr, &texture->data);
		if (FAILED(hr))
		{
			MessageBox(nullptr, L"[TEXTURE]\nFailed to create texture from memory", L"Error", MB_OK);
			return Err(hr);
		}
		
		return Ok(texture);
	};

	/// [TEXTURE] Load a texture from a specified path
	static Result<Texture*, HRESULT> Load(const std::string path)
	{
		HRESULT hr = S_OK;

		// Create a new texture instance to return
		Texture* pTextures = new Texture();
		pTextures->type = TextureType::CUSTOM_PATH;
		pTextures->path = path;

		// Convert const char* path to const wchar_t*
		const size_t pathStrSize = path.length() + 1;
		wchar_t* pathWstr = new wchar_t[pathStrSize];
		size_t convertedChars = 0;
		mbstowcs_s(&convertedChars, pathWstr, pathStrSize, path.c_str(), _TRUNCATE);

		// Load the texture into a temporary object
		hr = CreateDDSTextureFromFile(Graphics::Get().GetDevice(), pathWstr, nullptr, &pTextures->data);
		if (FAILED(hr))
		{
			MessageBox(nullptr, L"[TEXTURE]\nFailed to load DDS texture from file", L"Error", MB_OK);
			return Err(hr);
		}

		return Ok(pTextures);
	};

	/// [TEXTURE] Load multiple textures from an array of paths
	static Result<std::vector<Texture*>, HRESULT> Load(const std::vector<std::string> paths)
	{
		std::vector<Texture*> pTextures;

		for (const std::string path : paths)
		{
			Result<Texture*, HRESULT> textureResult = Load(path);
			if (textureResult.isErr())
				return Err(E_FAIL);

			pTextures.push_back(textureResult.unwrap()); // TODO: Safe unwrap
		}

		return Ok(pTextures);
	};
	
	/// [PATH] Path to the loaded texture
	aiString path;
	
	/// [TYPE] The type of texture
	TextureType type = TextureType::UNKNOWN;

	/// [TEXTURE] The loaded texture data as a shader resource view
	ID3D11ShaderResourceView* data = nullptr;
};
