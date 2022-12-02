#pragma once
#include "Module.h"
#include "glmath.h"
#include "Geometry/Frustum.h"
#include "Geometry/LineSegment.h"
#include "Geometry/Triangle.h"


class Camera3D : public Module
{
public:
	Camera3D(Application* app, bool start_enabled = true);
	~Camera3D();

	bool Start();
	update_status Update(float dt);
	void CheckInputs();
	bool CleanUp();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void Look();
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();
	void RecalculateProjection();
	void OrbitRotation();

	void FrontView();
	
	void MousePicking();

private:

	void CalculateViewMatrix();

	bool SaveConfig(JsonParser& node) const;

	bool LoadConfig(JsonParser& node);

public:
	float aspectRatio = 1.f;
	float verticalFOV = 100.f;
	float nearPlaneDistance = 0.1f;
	float farPlaneDistance = 1000.f;

	vec3 X, Y, Z, Position, Reference;

	Frustum cameraFrustum;

private:

	mat4x4 ViewMatrix, ViewMatrixInverse;

	ImVec2 normalizedMousePos;
	ImVec2 MousePos;
	ImVec2 WindowPos, WindowSize;

	float frameHeight;
};