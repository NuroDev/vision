#pragma once

#include <directxmath.h>
#include <imgui.h>

#include "../Config/Structrures.h"
#include "../Util/Structures.h"
#include "../Math/Transform.h"

using namespace DirectX;

class FreeCam
{
public:
	FreeCam() = default;

	FreeCam(CameraParameters* pParameters) : m_pParameters(pParameters) {};

	~FreeCam() { Release(); };

	/// [CAMERA] Initialize any core camera data
	HRESULT Init();

	/// [CAMERA] Draw the custom imgui ui to the screen
	void Ui();

	/// [CAMERA] Update the camera every tick
	void Update(float t);

	/// [CAMERA] Clean & release the camera when it's done
	void Release();

	/// [PARAMETERS] Get the camera's parameters object
	CameraParameters* GetParams() const { return m_pParameters; };

	/// [PARAMETERS] Set the camera's parameters object
	void SetParams(CameraParameters* pParams) { m_pParameters = pParams; };

	/// [TRANSFORM] Get the camera's transform
	Transform* GetTransform() { return m_pTransform; };

	/// [TRANSFORM] Set the camera's transform
	void SetTransform(Transform* transform) { m_pTransform = transform; };

	/// [TRANSFORM] Get the camera's projection matrix
	XMMATRIX GetProjection() { return m_projection; };

	/// [TRANSFORM] Set the camera's projection matrix
	void SetProjection(const XMMATRIX& matrix) { m_projection = matrix; };

	/// [TRANSFORM] Get the camera's view matrix
	XMMATRIX GetView() { return m_view; };

	/// [TRANSFORM] Set the camera's view matrix
	void SetView(XMVECTOR eye, XMVECTOR at, XMVECTOR up) { m_view = XMMatrixLookAtLH(eye, at, up); };

	/// [TRANSFORM] Set the camera's view matrix
	void SetView(const XMMATRIX& matrix) { m_view = matrix; };

private:
	/// [CAMERA] Synchronize the camera's position/rotation
	void Sync();

	/// [MISC]
	float m_movementSpeed = Defaults::Camera::Speed;

	/// [PARAMETERS]
	CameraParameters* m_pParameters = nullptr;

	/// [TRANSFORM]
	Transform* m_pTransform = nullptr;
	XMMATRIX m_projection;
	XMMATRIX m_rotation;
	XMMATRIX m_view;
	XMVECTOR m_target;
	XMVECTOR m_up;
};