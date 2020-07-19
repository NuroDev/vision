#pragma once

#include <d3d11.h>

#include "../Util/Structures.h"
#include "WorldObject.h"

class WorldLight : public WorldObject
{
public:
	WorldLight(
		std::string name,
		const unsigned int id,
		Transform* pTransform = new Transform()
	) : WorldObject(name, id, pTransform) {}
	
	~WorldLight() { WorldLight::Release(); };

	/// [OBJECT LIGHT] Initialize any core object data
	HRESULT Init() override;

	/// [OBJECT LIGHT] Draw the contents of the object to the screen
	void Draw() override;

	/// [OBJECT LIGHT] Draw the custom imgui ui to the screen
	void Ui() override;

	/// [OBJECT LIGHT] Update the object every tick
	void Update(float t) override;

	/// [OBJECT LIGHT] Cleanup & release the object when it is done
	void Release() override;

	/// [LIGHT] Get the light object
	Light GetLight() const { return m_light; };

	/// [LIGHT] Set the light object
	void SetLight(Light light) { m_light = light; };

private:
	/// [BUFFERS]
	ID3D11Buffer *m_pLightConstantBuffer = nullptr;

	/// [LIGHT]
	Light m_light;
};
