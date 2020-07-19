#pragma once

#include <directxmath.h>
#include <imgui.h>

#include "../Config/Structrures.h"
#include "../Util/Structures.h"

using namespace DirectX;

class FirstPersonCamera
{
public:
	FirstPersonCamera() = default;

	FirstPersonCamera(CameraParameters* pParameters) : m_pParameters(pParameters) {};
	
	~FirstPersonCamera() { Release(); };

	/// [CAMERA] Initialize any core camera data
	HRESULT Init();

	/// [CAMERA] Draw the custom imgui ui to the screen
	void Ui();

	/// [CAMERA] Update the camera every tick
	void Update(float t);

	/// [CAMERA] Clean & release the camera when it's done
	void Release();

	/// [CAMERA] Set the lens frustum
	void SetLens(
		const float fovY = Defaults::Camera::FOV,
		const float aspect = Defaults::Camera::AspectRatio,
		const float nearZ = Defaults::Camera::NearZ,
		const float farZ = Defaults::Camera::FarZ
	);

	/// [CAMERA] Set the camera space using a specified look at parameter
	void LookAt(FXMVECTOR position, FXMVECTOR target, FXMVECTOR worldUp);

	/// [CAMERA] Set the camera space using a specified look at parameter
	void LookAt(const XMFLOAT3& position, const XMFLOAT3& target, const XMFLOAT3& up);

	/// [CAMERA] Move sideways or "Strafe" the camera a specified distance
	void Strafe(float distance);

	/// [CAMERA] Move forward/backwards or "Walk" the camera a specified distance
	void Walk(float distance);

	/// [CAMERA] Rotate the camera pitch by a specified angle
	void Pitch(float angle);

	/// [CAMERA] Rotate the camera on the Y axis by a specified angle
	void RotateY(float angle);

	/// [PARAMETERS] Get the camera's parameters object
	[[nodiscard]] CameraParameters* GetParams() const { return m_pParameters; };

	/// [PARAMETERS] Set the camera's parameters object
	void SetParams(CameraParameters* pParams) { m_pParameters = pParams; };

	/// [MATRIX] Get the camera's projection matrix
	[[nodiscard]] XMMATRIX GetProjection() const { return XMLoadFloat4x4(&m_projection); };

	/// [MATRIX] Get the camera's view matrix
	[[nodiscard]] XMMATRIX GetView() const { return XMLoadFloat4x4(&m_view); };
	
private:
	float m_nearWindowHeight = 0.0f;
	float m_farWindowHeight = 0.0f;

	/// [MATRIX]
	XMFLOAT4X4 m_view;
	XMFLOAT4X4 m_projection;

	/// [PARAMETERS]
	CameraParameters* m_pParameters = nullptr;
};
