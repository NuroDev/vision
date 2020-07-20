#pragma once

#include "../Graphics/Graphics.hpp"
#include "../Math/Transform.hpp"
#include "../Model/Model.hpp"
#include "../Util/Structures.hpp"

using namespace DirectX;

class WorldObject
{
public:
	WorldObject() :
		m_name("Unnamed Object"),
		m_id(rand() % 100),
		m_pTransform(new Transform()),
		m_world()
	{};
	
	WorldObject(
		std::string name,
		const unsigned int id = rand() % 1000,
		Transform* pTransform = new Transform()
	) : m_name(name), m_id(id), m_pTransform(pTransform), m_world()
	{};
	virtual ~WorldObject() { WorldObject::Release(); };

	/// [WORLD OBJECT] Initialize any core object data
	virtual HRESULT Init();

	/// [WORLD OBJECT] Draw the contents of the object to the screen
	virtual void Draw();

	/// [WORLD OBJECT] Draw the custom imgui ui to the screen
	virtual void Ui();

	/// [WORLD OBJECT] Update the object every tick
	virtual void Update(float t);

	/// [WORLD OBJECT] Cleanup & release the object when it is done
	virtual void Release();

	/// [MISC] Get whether the object is enabled
	[[nodiscard]] bool GetEnabled() const { return m_enabled; };

	/// [MISC] Set whether the object is enabled
	void SetEnabled(bool val) { m_enabled = val; };
	
	/// [MISC] Get the registered name of the object
	[[nodiscard]] std::string GetName() const { return m_name; };

	/// [MISC] Set the registered name of the object
	void SetName(std::string name) { m_name = name; };

	/// [MISC] Get the registered id of the object
	[[nodiscard]] unsigned int GetID() const { return m_id; };

	/// [MISC] Set the registered id of the object
	void SetID(unsigned int id) { m_id = id; };

	/// [MODEL] Get the mesh from a specified index
	Mesh *GetMesh(unsigned int index) const { return m_pMeshes[index]; };

	/// [MODEL] Get all of the loaded meshes
	[[nodiscard]] std::vector<Mesh *> GetMesh() const { return m_pMeshes; };

	/// [MODEL] Set the mesh from a specified index
	void SetMesh(Mesh *pMesh, unsigned int index) { m_pMeshes[index] = pMesh; };

	/// [MODEL] Set the loaded meshes array
	void SetMesh(std::vector<Mesh *> pMeshes) { m_pMeshes = std::move(pMeshes); };

	/// [MODEL] Load meshes from paths specifying the mesh & textures
	HRESULT LoadModel(const std::string path);

	/// [SHADER] Get the input layout
	[[nodiscard]] ID3D11InputLayout *GetInputLayout() const { return m_pInputLayout; };

	/// [SHADER] Get the pixel shader
	[[nodiscard]] ID3D11PixelShader *GetPixelShader() const { return m_pPixelShader; };

	/// [SHADER] Get the vertex shader
	[[nodiscard]] ID3D11VertexShader *GetVertexShader() const { return m_pVertexShader; };

	/// [SHADER] Load a shader from a specified path
	HRESULT LoadShader(const char *path, const char *vsEntry = "VS", const char *psEntry = "PS");

	// TODO: Refector world object load shader to take an input layout as an argument
	/// [SHADER] Load a shader from a specified path
	HRESULT LoadShader(const wchar_t* path, const char* vsEntry = "VS", const char* psEntry = "PS");

	/// [SHADER] Set the input layout
	void SetInputLayout(D3D11_INPUT_ELEMENT_DESC layout, unsigned int index) { m_inputLayout[index] = layout; };

	/// [SHADER] Set the input layout
	void SetInputLayout(std::vector<D3D11_INPUT_ELEMENT_DESC> layout) { m_inputLayout = std::move(layout); };
	
	/// [TRANSFORM] Get the objects transform
	[[nodiscard]] Transform *GetTransform() const { return m_pTransform; };

	/// [TRANSFORM] Set the objects transform
	void SetTransform(Transform* pTransform) { m_pTransform = pTransform; };

	/// [TRANSFORM] Get the objects world matrix
	[[nodiscard]] XMMATRIX GetWorld() const { return m_world; };

	/// [TRANSFORM] Set the objects world matrix
	void SetWorld(XMMATRIX world) { m_world = world; };

private:
	/// [MODEL]
	std::vector<Mesh *> m_pMeshes;
	Model* m_pModel = nullptr;

	/// [MISC]
	bool m_enabled = true;
	std::string m_name;
	unsigned int m_id;

	/// [SHADER]
	std::vector<D3D11_INPUT_ELEMENT_DESC> m_inputLayout;
	ID3D11InputLayout *m_pInputLayout = nullptr;
	ID3D11PixelShader *m_pPixelShader = nullptr;
	ID3D11VertexShader *m_pVertexShader = nullptr;

	/// [TRANSFORM]
	Transform *m_pTransform = nullptr;
	XMMATRIX m_world;
};
