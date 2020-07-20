#include "FirstPersonCamera.hpp"

HRESULT FirstPersonCamera::Init()
{
	HRESULT hr = S_OK;

	if (m_pParameters == nullptr)
		m_pParameters = new CameraParameters();

	SetLens();

	return hr;
}

void FirstPersonCamera::Ui()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		if (ImGui::TreeNode("Transforms"))
		{
			if (ImGui::TreeNode("Position"))
			{
				ImGui::InputFloat("X", static_cast<float*>(&m_pParameters->position.x), 0.1f, 1.0f);
				ImGui::InputFloat("Y", static_cast<float*>(&m_pParameters->position.y), 0.1f, 1.0f);
				ImGui::InputFloat("Z", static_cast<float*>(&m_pParameters->position.z), 0.1f, 1.0f);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Rotation"))
			{
				ImGui::InputFloat3("Right", reinterpret_cast<float*>(&m_pParameters->right));
				ImGui::InputFloat3("Up", reinterpret_cast<float*>(&m_pParameters->up));
				ImGui::InputFloat3("Look", reinterpret_cast<float*>(&m_pParameters->look));
				ImGui::TreePop();
			}			

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Parameters"))
		{			
			ImGui::SliderFloat("", &m_pParameters->speed, 0.1f, 50.0f, "Speed: %.3f");
			ImGui::SameLine();
			if (ImGui::Button("Reset"))
				m_pParameters->speed = Defaults::Camera::Speed;

			ImGui::TreePop();
		}
	}
}

void FirstPersonCamera::Update(float t)
{
	// ================================================================
	// Update view matrix
	// ================================================================

	// Convert position/rotation 3x3 matrices to vectors
	XMVECTOR R = XMLoadFloat3(&m_pParameters->right);
	XMVECTOR U = XMLoadFloat3(&m_pParameters->up);
	XMVECTOR L = XMLoadFloat3(&m_pParameters->look);
	XMVECTOR P = XMLoadFloat3(&m_pParameters->position);

	// Keep camera's axes orthogonal to each other & of unit length
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));

	// U, L already ortho-normal, so no need to normalize cross product
	R = XMVector3Cross(U, L);

	// Fill in the view matrix entries
	const float x = -XMVectorGetX(XMVector3Dot(P, R));
	const float y = -XMVectorGetX(XMVector3Dot(P, U));
	const float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&m_pParameters->right, R);
	XMStoreFloat3(&m_pParameters->up, U);
	XMStoreFloat3(&m_pParameters->look, L);

	m_view(0, 0) = m_pParameters->right.x;
	m_view(1, 0) = m_pParameters->right.y;
	m_view(2, 0) = m_pParameters->right.z;
	m_view(3, 0) = x;

	m_view(0, 1) = m_pParameters->up.x;
	m_view(1, 1) = m_pParameters->up.y;
	m_view(2, 1) = m_pParameters->up.z;
	m_view(3, 1) = y;

	m_view(0, 2) = m_pParameters->look.x;
	m_view(1, 2) = m_pParameters->look.y;
	m_view(2, 2) = m_pParameters->look.z;
	m_view(3, 2) = z;

	m_view(0, 3) = 0.0f;
	m_view(1, 3) = 0.0f;
	m_view(2, 3) = 0.0f;
	m_view(3, 3) = 1.0f;

	// ================================================================
	// Update camera's properties
	// ================================================================

	// Multiply the sprint speed by delta time
	float speed = m_pParameters->speed * t;

	// "Sprint" movement
	if (GetAsyncKeyState(VK_SHIFT))
		speed += (speed / 0.5F);

	// Position
	if (GetAsyncKeyState('W'))
		Walk(speed);
	if (GetAsyncKeyState('S'))
		Walk(-speed);
	if (GetAsyncKeyState('A'))
		Strafe(-speed);
	if (GetAsyncKeyState('D'))
		Strafe(speed);
	if (GetAsyncKeyState('Q'))
		m_pParameters->position.y += -speed;
	if (GetAsyncKeyState('E'))
		m_pParameters->position.y += speed;

	// Rotation
	if (GetAsyncKeyState(VK_UP))
		Pitch(-speed);
	if (GetAsyncKeyState(VK_DOWN))
		Pitch(speed);
	if (GetAsyncKeyState(VK_LEFT))
		RotateY(-speed);
	if (GetAsyncKeyState(VK_RIGHT))
		RotateY(speed);

	// Reset
	if (GetAsyncKeyState('R'))
	{
		m_pParameters->position = Defaults::Camera::Position;
		m_pParameters->right = Defaults::Camera::Right;
		m_pParameters->up = Defaults::Camera::Up;
		m_pParameters->look = Defaults::Camera::Look;
	}
}

void FirstPersonCamera::Release()
{
	if (m_pParameters)
	{
		delete m_pParameters;
		m_pParameters = nullptr;
	}
}

void FirstPersonCamera::SetLens(const float fovY, const float aspect, const float nearZ, const float farZ)
{
	// Save parameters
	m_pParameters->fovY = fovY;
	m_pParameters->aspectRatio = aspect;
	m_pParameters->nearZ = nearZ;
	m_pParameters->farZ = farZ;

	// Update the window heights using the updated parameters
	m_nearWindowHeight = 2.0f * m_pParameters->nearZ * tanf(0.5f * m_pParameters->fovY);
	m_farWindowHeight = 2.0f * m_pParameters->farZ * tanf(0.5f * m_pParameters->fovY);

	// Update the projection matrix
	const XMMATRIX P = XMMatrixPerspectiveFovLH(m_pParameters->fovY, m_pParameters->aspectRatio, m_pParameters->nearZ, m_pParameters->farZ);
	XMStoreFloat4x4(&m_projection, P);
}

void FirstPersonCamera::LookAt(FXMVECTOR position, FXMVECTOR target, FXMVECTOR worldUp)
{
	// Update position matrix
	XMStoreFloat3(&m_pParameters->position, position);

	// Update look at vector
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, position));
	XMStoreFloat3(&m_pParameters->look, L);

	// Update right vector
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMStoreFloat3(&m_pParameters->right, R);

	// Update up vector
	XMVECTOR U = XMVector3Cross(L, R);	
	XMStoreFloat3(&m_pParameters->up, U);
}

void FirstPersonCamera::LookAt(const XMFLOAT3& position, const XMFLOAT3& target, const XMFLOAT3& up)
{
	LookAt(
		XMLoadFloat3(&position),	// P
		XMLoadFloat3(&target),		// T
		XMLoadFloat3(&up)			// U
	);
}

void FirstPersonCamera::Strafe(float d)
{
	// Move the camera sideways, AKA: Strafe
	// (position += distance * right)

	XMStoreFloat3(
		&m_pParameters->position,					// P
		XMVectorMultiplyAdd(
			XMVectorReplicate(d),					// S		
			XMLoadFloat3(&m_pParameters->right),	// R
			XMLoadFloat3(&m_pParameters->position)	// P
		)
	);
}

void FirstPersonCamera::Walk(float d)
{
	// Move the player forward or backward, AKA: Walking
	// position += distance * LookAt

	XMStoreFloat3(
		&m_pParameters->position,					// P
		XMVectorMultiplyAdd(
			XMVectorReplicate(d),					// S
			XMLoadFloat3(&m_pParameters->look),		// L
			XMLoadFloat3(&m_pParameters->position)	// P
		)
	);
}

void FirstPersonCamera::Pitch(float angle)
{
	// Rotate up & look vectors about the right vector
	const XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_pParameters->right), angle);
	
	XMStoreFloat3(&m_pParameters->up, XMVector3TransformNormal(XMLoadFloat3(&m_pParameters->up), R));
	XMStoreFloat3(&m_pParameters->look, XMVector3TransformNormal(XMLoadFloat3(&m_pParameters->look), R));
}

void FirstPersonCamera::RotateY(float angle)
{
	// Rotate the basis vectors about the world y-axis
	const XMMATRIX R = XMMatrixRotationY(angle);
	XMStoreFloat3(&m_pParameters->right, XMVector3TransformNormal(XMLoadFloat3(&m_pParameters->right), R));
	XMStoreFloat3(&m_pParameters->up, XMVector3TransformNormal(XMLoadFloat3(&m_pParameters->up), R));
	XMStoreFloat3(&m_pParameters->look, XMVector3TransformNormal(XMLoadFloat3(&m_pParameters->look), R));
}
