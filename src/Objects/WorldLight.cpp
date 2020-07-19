#include "WorldLight.h"

HRESULT WorldLight::Init()
{
	HRESULT hr = S_OK;

	// Get the graphics instance
	Graphics& graphics = Graphics::Get();

	// Create the light constant buffer
	D3D11_BUFFER_DESC bd = {};
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(LightBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = graphics.GetDevice()->CreateBuffer(&bd, nullptr, &m_pLightConstantBuffer);
	if (FAILED(hr))
		return hr;

	// Initialize the meshes
	for (Mesh *pMesh : WorldObject::GetMesh())
	{
		hr = pMesh->Init();
		if (FAILED(hr))
			return hr;
	}

	return hr;
}

void WorldLight::Draw()
{
	// Get the graphics instance
	Graphics& graphics = Graphics::Get();

	// Get the world object transform
	XMFLOAT3 position = GetTransform()->GetPosition();
	
	// TODO: Nicer way to convert from XMFLOAT3 to XMFLOAT4
	// Set the light position
	m_light.Position = XMFLOAT4(position.x, position.y, position.z, 1.0f);

	// Calculate the light direction
	XMVECTOR LightDirection = XMVectorSet(-m_light.Position.x, -m_light.Position.y, -m_light.Position.z, 0.0f);
	LightDirection = XMVector3Normalize(LightDirection);
	XMStoreFloat4(&m_light.Direction, LightDirection);

	// Create light constant buffer
	LightBuffer lightProperties;
	lightProperties.EyePosition = m_light.Position;
	lightProperties.Lights[0] = m_light;

	// Update light constant buffer using created light instance
	graphics.GetContext()->UpdateSubresource(m_pLightConstantBuffer, 0, nullptr, &lightProperties, 0, 0);

	// Set light constant buffer
	graphics.GetContext()->PSSetConstantBuffers(2, 1, &m_pLightConstantBuffer);

	// Draw mesh(s)
	//for (Mesh* pMesh : WorldObject::GetMesh())
	//	pMesh->Draw();
}

void WorldLight::Ui()
{
	if (ImGui::CollapsingHeader("Light"))
	{
		if (ImGui::TreeNode("Parameters"))
		{
			ImGui::Checkbox("Enabled", reinterpret_cast<bool*>(&m_light.Enabled));
			ImGui::Spacing();

			// TODO: Fix
			if (ImGui::RadioButton("Directional", m_light.LightType == LightType::DirectionalLight)) { m_light.LightType = LightType::DirectionalLight; } ImGui::SameLine();
			if (ImGui::RadioButton("Point", m_light.LightType == LightType::PointLight)) { m_light.LightType = LightType::PointLight; } ImGui::SameLine();
			if (ImGui::RadioButton("Spot", m_light.LightType == LightType::SpotLight)) { m_light.LightType = LightType::SpotLight; }

			// TODO: Fix
			ImGui::SliderFloat("Spot Angle", &m_light.SpotAngle, 1.0f, 90.0f);
			ImGui::Spacing();

			ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&m_light.Color));
			ImGui::Spacing();
			
			ImGui::Text("Attenuation");
			ImGui::Separator();
			
			ImGui::SliderFloat("Constant", &m_light.ConstantAttenuation, 0, 1);
			ImGui::Spacing();

			ImGui::SliderFloat("Linear", &m_light.LinearAttenuation, 0, 1);
			ImGui::Spacing();

			ImGui::SliderFloat("Quadratic", &m_light.QuadraticAttenuation, 0, 1);
			ImGui::Spacing();

			ImGui::TreePop();
		}
		
		if (ImGui::TreeNode("Transforms"))
		{
			if (ImGui::TreeNode("Position"))
			{
				Transform* pTransform = GetTransform();
				XMFLOAT3 currentPosition = pTransform->GetPosition();
				ImGui::SliderFloat("X", static_cast<float*>(&currentPosition.x), -10.0f, 10.0f);
				ImGui::SliderFloat("Y", static_cast<float*>(&currentPosition.y), -10.0f, 10.0f);
				ImGui::SliderFloat("Z", static_cast<float*>(&currentPosition.z), -10.0f, 10.0f);
				pTransform->SetPosition(currentPosition);
				SetTransform(pTransform);
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}
	}
}

void WorldLight::Update(float)
{	
	// Reset transform
	if (GetAsyncKeyState('R'))
	{
		Transform* pTransform = GetTransform();
		pTransform->SetPosition(Defaults::Light::Position);
		pTransform->SetRotation(Defaults::Light::Rotation);
		SetTransform(pTransform);
	}
}

void WorldLight::Release()
{
	SAFE_RELEASE(m_pLightConstantBuffer);
	WorldObject::Release();
}
