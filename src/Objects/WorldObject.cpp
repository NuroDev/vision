#include "WorldObject.h"

HRESULT WorldObject::Init()
{
	HRESULT hr = S_OK;

	if (m_pPixelShader == nullptr || m_pInputLayout == nullptr || m_pVertexShader == nullptr)
	{
		MessageBox(nullptr, L"[WORLD OBJECT]\nNo shader has been initialized. Please make sure a shader has been loaded before initializing.", L"Error", MB_OK);
		return hr;
	}

	if (m_pMeshes.empty())
	{
		MessageBox(nullptr, L"[WORLD OBJECT]\nNo meshes have been loaded. Please make sure at least one mesh has been loaded before initializing.", L"Error", MB_OK);
		return hr;
	}

	for (Mesh *pMesh : m_pMeshes)
		pMesh->Init();

	return hr;
}

void WorldObject::Draw()
{
	// Check if the object is enabled
	if (!m_enabled)
		return;
	
	// Get the graphics instance
	Graphics& graphics = Graphics::Get();

	// Get the constant buffer
	ConstantBuffer& constantBuffer = ConstantBuffer::Get();

	// Update the constant buffer world matrix
	constantBuffer.SetWorld(XMMatrixTranspose(m_world));

	// Update constant buffer with new world matrix
	const ConstantBufferData bufferData = constantBuffer.GetData();
	graphics.GetContext()->UpdateSubresource(graphics.GetConstantBuffer(), 0, nullptr, &bufferData, 0, 0);

	if (!graphics.GetParams()->doDeferred)
	{
		// Set shaders
		graphics.GetContext()->VSSetShader(m_pVertexShader, nullptr, 0);
		graphics.GetContext()->PSSetShader(m_pPixelShader, nullptr, 0);

		// Set input layout
		graphics.GetContext()->IASetInputLayout(m_pInputLayout);
	}

	// Draw the model
	m_pModel->Draw();
}

void WorldObject::Ui()
{
	std::string fullname = m_name + " (" + std::to_string(m_id) + ")";
	
	if (ImGui::CollapsingHeader(fullname.c_str()))
	{
		if (ImGui::TreeNode("Parameters"))
		{
			ImGui::Checkbox("Enabled", &m_enabled);
			ImGui::TreePop();
		}

		m_pModel->Ui();

		if (ImGui::TreeNode("Transforms"))
		{
			if (ImGui::TreeNode("Position"))
			{
				XMFLOAT3 currentPosition = m_pTransform->GetPosition();
				ImGui::SliderFloat("X", static_cast<float *>(&currentPosition.x), -5.0f, 10.0f);
				ImGui::SliderFloat("Y", static_cast<float *>(&currentPosition.y), -5.0f, 10.0f);
				ImGui::SliderFloat("Z", static_cast<float *>(&currentPosition.z), -5.0f, 10.0f);
				m_pTransform->SetPosition(currentPosition);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Rotation"))
			{
				XMFLOAT3 currentRotation = m_pTransform->GetRotation();
				ImGui::SliderFloat("X", static_cast<float *>(&currentRotation.x), 0.0f, 360.0f);
				ImGui::SliderFloat("Y", static_cast<float *>(&currentRotation.y), 0.0f, 360.0f);
				ImGui::SliderFloat("Z", static_cast<float *>(&currentRotation.z), 0.0f, 360.0f);
				m_pTransform->SetRotation(currentRotation);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Scale"))
			{
				XMFLOAT3 currentScale = m_pTransform->GetScale();
				ImGui::SliderFloat("X", static_cast<float *>(&currentScale.x), 0.0f, 10.0f);
				ImGui::SliderFloat("Y", static_cast<float *>(&currentScale.y), 0.0f, 10.0f);
				ImGui::SliderFloat("Z", static_cast<float *>(&currentScale.z), 0.0f, 10.0f);
				m_pTransform->SetScale(currentScale);
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}
	}
}

void WorldObject::Update(float)
{
	const XMMATRIX scale = XMMatrixScaling(m_pTransform->GetScale().x, m_pTransform->GetScale().y, m_pTransform->GetScale().z);
	const XMMATRIX rotation = XMMatrixRotationX(m_pTransform->GetRotation().x) * XMMatrixRotationY(m_pTransform->GetRotation().y) * XMMatrixRotationZ(m_pTransform->GetRotation().z);
	const XMMATRIX translation = XMMatrixTranslation(m_pTransform->GetPosition().x, m_pTransform->GetPosition().y, m_pTransform->GetPosition().z);

	m_world = scale * rotation * translation;
}

void WorldObject::Release()
{
	for (Mesh *pMesh : m_pMeshes)
		SAFE_RELEASE(pMesh);
	SAFE_RELEASE(m_pModel);

	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pVertexShader);

	if (m_pTransform)
	{
		delete m_pTransform;
		m_pTransform = nullptr;
	}
}

HRESULT WorldObject::LoadModel(const std::string path)
{
	HRESULT hr = S_OK;

	if (m_pModel == nullptr)
		m_pModel = new Model();
	
	hr = m_pModel->Load(path);
	if (FAILED(hr))
		return hr;

	const std::vector<Mesh*> pMeshes = m_pModel->GetMesh();
	
	if (pMeshes.empty())
		return E_FAIL;
		
	m_pMeshes = pMeshes;
	
	return hr;
}

HRESULT WorldObject::LoadShader(const char* path, const char* vsEntry, const char* psEntry)
{
	HRESULT hr = S_OK;

	hr = LoadShader(Helpers::StrToWstr(path).c_str(), vsEntry, psEntry);
	if (FAILED(hr))
		return hr;
	
	return hr;
}

HRESULT WorldObject::LoadShader(const wchar_t *path, const char *vsEntry, const char *psEntry)
{
	HRESULT hr = S_OK;

	// Get graphics instance
	Graphics& graphics = Graphics::Get();

	// Compile the vertex shader
	ID3DBlob *pVSBlob = nullptr;
	hr = Helpers::CompileShaderFromFile(path, vsEntry, "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"The shader file's vertex entry cannot be compiled. Please validate the shader code.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = graphics.GetDevice()->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_pVertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// TODO: Move this to constants class
	// Failover input layout
	if (m_inputLayout.empty())
		m_inputLayout = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	
	// Create the input layout
	const UINT inputLayoutSize = static_cast<UINT>(m_inputLayout.size());
	hr = graphics.GetDevice()->CreateInputLayout(m_inputLayout.data(), inputLayoutSize, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pInputLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;
	graphics.GetContext()->IASetInputLayout(m_pInputLayout);

	// Compile the pixel shader
	ID3DBlob *pPSBlob = nullptr;
	hr = Helpers::CompileShaderFromFile(path, psEntry, "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"The shader file's pixel entry cannot be compiled. Please validate the shader code.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = graphics.GetDevice()->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pPixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	return hr;
}
