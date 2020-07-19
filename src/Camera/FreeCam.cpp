#include "FreeCam.h"

HRESULT FreeCam::Init()
{
	HRESULT hr = S_OK;

	if (m_pParameters == nullptr)
		m_pParameters = new CameraParameters();

	m_projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_pParameters->aspectRatio, m_pParameters->nearZ, m_pParameters->farZ);

	m_pTransform = new Transform(Defaults::Camera::Position, Defaults::Transform::Rotation);

	Sync();

	return hr;
}

void FreeCam::Ui()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		if (ImGui::TreeNode("Transforms"))
		{
			if (ImGui::TreeNode("Position"))
			{
				XMFLOAT3 currentPosition = m_pTransform->GetPosition();
				ImGui::SliderFloat("X", static_cast<float*>(&currentPosition.x), -5.0f, 5.0f);
				ImGui::SliderFloat("Y", static_cast<float*>(&currentPosition.y), -5.0f, 5.0f);
				ImGui::SliderFloat("Z", static_cast<float*>(&currentPosition.z), -5.0f, 5.0f);
				m_pTransform->SetPosition(currentPosition);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Rotation"))
			{
				XMFLOAT3 currentRotation = m_pTransform->GetRotation();
				ImGui::SliderFloat("X", static_cast<float*>(&currentRotation.x), -5.0f, 5.0f);
				ImGui::SliderFloat("Y", static_cast<float*>(&currentRotation.y), -5.0f, 5.0f);
				ImGui::SliderFloat("Z", static_cast<float*>(&currentRotation.z), -5.0f, 5.0f);
				m_pTransform->SetRotation(currentRotation);
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Parameters"))
		{
			ImGui::SliderFloat("", &m_movementSpeed, 0.1f, 50.0f, "Speed: %.3f");
			ImGui::SameLine();
			if (ImGui::Button("Reset"))
				m_movementSpeed = Defaults::Camera::Speed;

			ImGui::TreePop();
		}
	}
}

void FreeCam::Update(float t)
{
	Sync();

	float speed = m_movementSpeed * t;

	// "Sprint" movement
	if (GetAsyncKeyState(VK_SHIFT))
		speed += (speed / 0.5f);

	// Position
	if (GetAsyncKeyState('W'))
		m_pTransform->IncreasePosition(0.0f, 0.0f, speed);
	if (GetAsyncKeyState('S'))
		m_pTransform->IncreasePosition(0.0f, 0.0f, -speed);
	if (GetAsyncKeyState('A'))
		m_pTransform->IncreasePosition(-speed, 0.0f, 0.0f);
	if (GetAsyncKeyState('D'))
		m_pTransform->IncreasePosition(speed, 0.0f, 0.0f);
	if (GetAsyncKeyState('Q'))
		m_pTransform->IncreasePosition(0.0f, -speed, 0.0f);
	if (GetAsyncKeyState('E'))
		m_pTransform->IncreasePosition(0.0f, speed, 0.0f);

	// Rotation
	if (GetAsyncKeyState(VK_UP))
		m_pTransform->IncreaseRotation(-speed, 0.0f, 0.0f);
	if (GetAsyncKeyState(VK_DOWN))
		m_pTransform->IncreaseRotation(speed, 0.0f, 0.0f);
	if (GetAsyncKeyState(VK_LEFT))
		m_pTransform->IncreaseRotation(0.0f, -speed, 0.0f);
	if (GetAsyncKeyState(VK_RIGHT))
		m_pTransform->IncreaseRotation(0.0f, speed, 0.0f);

	// Reset
	if (GetAsyncKeyState('R'))
	{
		m_pTransform->SetPosition(Defaults::Camera::Position);
		m_pTransform->SetRotation(Defaults::Transform::Rotation);
	}
}

void FreeCam::Release()
{
	if (m_pTransform)
	{
		delete m_pTransform;
		m_pTransform = nullptr;
	}
}

void FreeCam::Sync()
{
	// Calculate camera rotation from transform
	m_rotation = XMMatrixRotationRollPitchYaw(
		m_pTransform->GetRotation().x,
		m_pTransform->GetRotation().y,
		m_pTransform->GetRotation().z);

	// Get current transform position
	XMFLOAT3 position = m_pTransform->GetPosition();

	// Set camera target (Look at point)
	m_target = XMVector3TransformCoord(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), m_rotation);
	m_target += XMLoadFloat3(&position);

	// Set camera up direction
	m_up = XMVector3TransformCoord(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), m_rotation);

	// Set view matrix
	m_view = XMMatrixLookAtLH(XMLoadFloat3(&position), m_target, m_up);
}
