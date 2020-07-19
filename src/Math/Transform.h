#pragma once

#include <DirectXCollision.h>

#include "../Config/Defaults.h"

using namespace DirectX;

class Transform
{
public:
	Transform(XMFLOAT3 position = Defaults::Transform::Position, XMFLOAT3 rotation = Defaults::Transform::Rotation, XMFLOAT3 scale = Defaults::Transform::Scale) : position(position),
																																									  rotation(rotation),
																																									  scale(scale){};

	/// [POSITION] Get the current position vector
	XMFLOAT3 GetPosition() { return position; };

	/// [POSITION] Set the current position vector
	void SetPosition(XMFLOAT3 p) { position = p; }

	/// [POSITION] Set the current position vector
	void SetPosition(float x, float y, float z) { position = XMFLOAT3(x, y, z); }

	/// [POSITION] Increase the position across a certain axis
	void IncreasePosition(float x, float y, float z)
	{
		position.x += x;
		position.y += y;
		position.z += z;
	};

	/// [POSITION] Decrease the position across a certain axis
	void DecreasePosition(float x, float y, float z)
	{
		position.x -= x;
		position.y -= y;
		position.z -= z;
	};

	/// [ROTATION] Get the current rotation vector
	XMFLOAT3 GetRotation() { return rotation; };

	/// [ROTATION] Set the current rotation vector
	void SetRotation(XMFLOAT3 r) { rotation = r; }

	/// [ROTATION] set the current rotation vector
	void SetRotation(float x, float y, float z) { rotation = XMFLOAT3(x, y, z); }

	/// [ROTATION] Increase the rotation across a certain axis
	void IncreaseRotation(float x, float y, float z)
	{
		rotation.x += x;
		rotation.y += y;
		rotation.z += z;
	};

	/// [ROTATION] Decrease the rotation across a certain axis
	void DecreaseRotation(float x, float y, float z)
	{
		rotation.x -= x;
		rotation.y -= y;
		rotation.z -= z;
	};

	/// [SCALE] Get the current scale vector
	XMFLOAT3 GetScale() { return scale; };

	/// [SCALE] Set the current scale vector
	void SetScale(XMFLOAT3 s) { scale = s; }

	/// [SCALE] Set the current scale vector
	void SetScale(float x, float y, float z) { scale = XMFLOAT3(x, y, z); }

	/// [SCALE] Increase the scale across a certain axis
	void IncreaseScale(float x, float y, float z)
	{
		scale.x += x;
		scale.y += y;
		scale.z += z;
	};

	/// [SCALE] Decrease the scale across a certain axis
	void DecreaseScale(float x, float y, float z)
	{
		scale.x -= x;
		scale.y -= y;
		scale.z -= z;
	};

private:
	XMFLOAT3 position = Defaults::Transform::Position;
	XMFLOAT3 rotation = Defaults::Transform::Rotation;
	XMFLOAT3 scale = Defaults::Transform::Scale;
};
