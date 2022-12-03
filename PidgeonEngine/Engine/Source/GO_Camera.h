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

	void CalculateViewMatrix();
	void RecalculateProjection();

	void OnEditor();

	bool SaveConfig(JsonParser& node) const;

	bool LoadConfig(JsonParser& node);

public:
	float aspectRatio = 1.f;
	float verticalFOV = 100.f;
	float nearPlaneDistance = 0.1f;
	float farPlaneDistance = 250.f;

	float3 X, Y, Z, Position, Reference;

	Frustum cameraFrustum;

	float4x4 ViewMatrix;
	
};