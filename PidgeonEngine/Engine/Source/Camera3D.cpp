#include "Application.h"
#include "Camera3D.h"
#include "Globals.h"
#include "Input.h"
#include "Editor.h"
#include "GameObject.h"
#include "Transform.h"


#include "Math/float4x4.h"
#include "Math/float3.h"

Camera3D::Camera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Camera3D";

	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);
}

Camera3D::~Camera3D()
{}

bool Camera3D::Start()
{
	bool ret = true;

	return ret;
}

bool Camera3D::CleanUp()
{
	LOG(LogType::L_NO_PRINTABLE, "Cleaning camera");

	return true;
}

update_status Camera3D::Update(float dt)
{

	return UPDATE_CONTINUE;
}

void Camera3D::CheckInputs()
{
	vec3 newPos(0, 0, 0);
	float speed = 3.0f * app->GetDt();
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 8.0f * app->GetDt();

	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_REPEAT) newPos.y += speed;
	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_REPEAT) newPos.y -= speed;

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) FrontView();

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;


	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

	newPos -= Z * App->input->GetWheel();

	Position += newPos;
	Reference += newPos;

	// Mouse motion ----------------
	OrbitRotation();

	// Recalculate matrix -------------
	CalculateViewMatrix();
}

void Camera3D::OrbitRotation()
{
	vec3 pivot = vec3(0, 0, 0);
	GameObject* gameObject = App->editor->GetGameObjectSelected();

	float3 posGO = { 0, 0, 0 };

	if (gameObject != nullptr)posGO = gameObject->transform->GetPosition();

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();
		float Sensitivity = 0.25f;
		app->editor->GetGameObjectSelected();
		(App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && gameObject != nullptr) ? pivot = vec3(posGO.x, posGO.y, posGO.z) : pivot = Reference;

		Position -= pivot;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			Y = rotate(Y, DeltaY, X);
			Z = rotate(Z, DeltaY, X);

			if (Y.y < 0.0f)
			{
				Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = cross(Z, X);
			}
		}
		Position = pivot + Z * length(Position);
		Reference = pivot;

	}
}

void Camera3D::FrontView()
{
	GameObject* gameObject = App->editor->GetGameObjectSelected();
	float3 posGO = { 0, 0, 0 };
	vec3 newPos;

	//float dist = gameObject->components.a;


	if (gameObject != nullptr)
	{
		posGO = gameObject->transform->GetPosition();

		newPos = vec3(posGO.x, posGO.y, posGO.z);
		Position = newPos + vec3(0, 0, -10);
		LookAt(newPos);
	}
}

//TODO: add cameraFrustum & add interection test (AABB)
void Camera3D::MousePicking()
{
	MousePos.x = (float)app->input->GetMouseX();
	MousePos.y = (float)app->input->GetMouseY();

	WindowPos = ImGui::GetWindowPos();
	WindowSize = ImGui::GetWindowSize();
	frameHeight = ImGui::GetFrameHeight();


	normalizedMousePos.x = -1.0 + 2.0 * ((MousePos.x - WindowPos.x) / ((WindowPos.x + WindowSize.x) - WindowPos.x));
	normalizedMousePos.y = 1.0 - 2.0 * ((MousePos.y - (WindowPos.y + frameHeight)) / (((WindowPos.y + frameHeight) + (WindowSize.y - frameHeight)) - (WindowPos.y + frameHeight)));

	LineSegment picking = cameraFrustum.UnProjectLineSegment(normalizedMousePos.x, normalizedMousePos.y);

}

// -----------------------------------------------------------------
void Camera3D::Look(const vec3& Position, const vec3& Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

void Camera3D::Look()
{

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void Camera3D::LookAt(const vec3& Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void Camera3D::Move(const vec3& Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* Camera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

// -----------------------------------------------------------------
void Camera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}
bool Camera3D::SaveConfig(JsonParser& node) const
{
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "X.x", X.x);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "X.y", X.y);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "X.z", X.z);

	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Y.x", Y.x);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Y.y", Y.y);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Y.z", Y.z);

	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Z.x", Z.x);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Z.y", Z.y);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Z.z", Z.z);

	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Reference.x", Reference.x);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Reference.y", Reference.y);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Reference.z", Reference.z);

	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Position.x", Position.x);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Position.y", Position.y);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Position.z", Position.z);

	return true;
}

bool Camera3D::LoadConfig(JsonParser& node)
{
	LOG(LogType::L_NORMAL, "Setting up the camera");

	X.x = (float)node.JsonValToNumber("X.x");
	X.y = (float)node.JsonValToNumber("X.y");
	X.z = (float)node.JsonValToNumber("X.z");

	Y.x = (float)node.JsonValToNumber("Y.x");
	Y.y = (float)node.JsonValToNumber("Y.y");
	Y.z = (float)node.JsonValToNumber("Y.z");

	Z.x = (float)node.JsonValToNumber("Z.x");
	Z.y = (float)node.JsonValToNumber("Z.y");
	Z.z = (float)node.JsonValToNumber("Z.z");

	Position.x = (float)node.JsonValToNumber("Position.x");
	Position.y = (float)node.JsonValToNumber("Position.y");
	Position.z = (float)node.JsonValToNumber("Position.z");

	Reference.x = (float)node.JsonValToNumber("Reference.x");
	Reference.y = (float)node.JsonValToNumber("Reference.y");
	Reference.z = (float)node.JsonValToNumber("Reference.z");

	Look();
	//CalculateViewMatrix();

	return true;
}