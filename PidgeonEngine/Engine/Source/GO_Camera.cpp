#include "Application.h"
#include "MeshRenderer.h"

#include "Renderer3D.h"
#include "Scene.h"

#include "GameObject.h"
#include "Transform.h"
#include "GO_Camera.h"

#include "ImGui/imgui.h"

GO_Camera::GO_Camera(GameObject* obj) : Component(nullptr)
{
	CalculateViewMatrix();

	X = float3(1.0f, 0.0f, 0.0f);
	Y = float3(0.0f, 1.0f, 0.0f);
	Z = float3(0.0f, 0.0f, 1.0f);

	Position = float3(0.0f, 0.0f, 15.0f);
	Reference = float3(0.0f, 0.0f, 0.0f);

	float3 size(1, 1, 1);
	Quat rotationQuat(0, 0, 0, 1);

	//owner->transform->SetTransformMatrix(Position, rotationQuat, size, nullptr);

	CalculateViewMatrix();

	LookAt(float3::zero);
}

GO_Camera::~GO_Camera()
{
}

void GO_Camera::LookAt(const float3& Spot)
{
	Reference = Spot;

	Z = (Reference-Position).Normalized();
	X = float3(0.0f, 1.0f, 0.0f).Cross(Z).Normalized();
	Y = Z.Cross(X);

	CalculateViewMatrix();
}

void GO_Camera::CalculateViewMatrix()
{
	cameraFrustum.pos = Position;
	cameraFrustum.front = Z.Normalized();
	cameraFrustum.up = Y.Normalized();
	float3::Orthonormalize(cameraFrustum.front, cameraFrustum.up);
	X = Y.Cross(Z);
	ViewMatrix = cameraFrustum.ViewMatrix();
}

void GO_Camera::RecalculateProjection()
{ 
	cameraFrustum.type = FrustumType::PerspectiveFrustum;
	cameraFrustum.nearPlaneDistance = nearPlaneDistance;
	cameraFrustum.farPlaneDistance = farPlaneDistance;
	cameraFrustum.verticalFov = (verticalFOV * 3.141592 / 2) / 180.f;
	cameraFrustum.horizontalFov = 2.f * atanf(tanf(cameraFrustum.verticalFov * 0.5f) * aspectRatio);
}

void GO_Camera::OnEditor()
{
	if (ImGui::CollapsingHeader("Camera Editor"))
	{
		if (ImGui::DragFloat("Vertical FOV", &verticalFOV))
		{
			RecalculateProjection();
		}
		if (ImGui::DragFloat("Near Plane Distance", &nearPlaneDistance))
		{
			RecalculateProjection();
		}
		if (ImGui::DragFloat("Far Plane Distance", &farPlaneDistance))
		{
			RecalculateProjection();
		}
	}
}

void GO_Camera::DrawCamera()
{
	Position = owner->transform->GetPosition();

	CalculateViewMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(cameraFrustum.ProjectionMatrix().Transposed().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(ViewMatrix.Transposed().ptr());

}

void GO_Camera::DrawCameraBounds()
{
	float3 corners[8];
	cameraFrustum.GetCornerPoints(corners);
	unsigned int index[24] = { 0,2, 2,6, 6,4, 4,0, 0,1, 1,3, 3,2, 3,7, 7,5, 5,1, 6,7, 5,4 };

	glBegin(GL_LINES);
	for (int i = 0; i < 24; i++)
	{
		glVertex3fv(&corners[index[i]].x);
	}
	glEnd();
}