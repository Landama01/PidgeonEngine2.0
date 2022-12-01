#pragma once

#include "Component.h"

#include "Math/float3.h"
#include "Math/float4x4.h"

class GO_Camera : public Component
{
public:
	GO_Camera(GameObject* obj);
	virtual ~GO_Camera();

	void LookAt(const float3& Spot);
	void DrawCamera();
	void DrawCameraBounds();

private:

	void CalculateViewMatrix();

	bool SaveConfig(JsonParser& node) const;

	bool LoadConfig(JsonParser& node);

public:

	float3 X, Y, Z, Position, Reference;

	Frustum cameraFrustum;

	float4x4 ViewMatrix;
	
};