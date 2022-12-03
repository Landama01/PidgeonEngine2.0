#include "Transform.h"
#include "Globals.h"
#include "MeshRenderer.h"
#include "ResourceMesh.h"

#include "ImGui/imgui.h"
#include "MathGeoLib/include/Math/TransformOps.h"

Transform::Transform(GameObject* obj) : Component(obj), updateTransform(false)
{
	// Inicialize transforms
	globalTransform.SetIdentity();
	localTransform.SetIdentity();

	localTransform.Decompose(position, rotation, scale);

	eulerRotation = rotation.ToEulerXYZ();

	globalTransformTransposed = globalTransform.Transposed();
}

Transform::~Transform()
{
}

void Transform::Update()
{
}

void Transform::OnEditor()
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Position: ");
		if (ImGui::DragFloat3("##Position", &position[0], 0.1f))
		{
			// Only overwrite position
			localTransform.SetCol3(3, position);
			updateTransform = true;
		}

		ImGui::Text("Rotation: ");
		if (ImGui::DragFloat3("##Rotation", &eulerRotation[0], 0.1f))
		{
			if (eulerRotation[0] == 0) eulerRotation[0] = 0;
			if (eulerRotation[2] == 0) eulerRotation[2] = 0;

			// Calculate quaternion
			rotation = Quat::FromEulerXYZ(eulerRotation.x * DEGTORAD, eulerRotation.y * DEGTORAD, eulerRotation.z * DEGTORAD);

			if (localTransform.Trace() == 0)
				localTransform.SetRotatePart(rotation);
			else
				localTransform.SetRotatePart(float3x3::FromRS(rotation, scale));

			updateTransform = true;
		}

		ImGui::Text("Scale: ");
		if (ImGui::DragFloat3("##Scale", &scale[0], 0.1f))
		{
			if (rotation.Equals(Quat::identity))
				localTransform.Scale(scale);
			else
				localTransform.SetRotatePart(float3x3::FromRS(rotation, scale));

			updateTransform = true;
		}
		if (updateTransform)
			UpdateTransform();
	}
}

void Transform::UpdateTransform()
{
	std::vector<Transform*> transformsToUpdate;
	GetRecursiveTransforms(this, transformsToUpdate);

	for (size_t i = 0; i < transformsToUpdate.size(); i++)
	{
		if (transformsToUpdate[i]->GetOwner()->GetParent() != nullptr)
		{
			Transform* parentTra = transformsToUpdate[i]->GetOwner()->GetParent()->transform;

			if (parentTra != nullptr) {
				transformsToUpdate[i]->globalTransform = parentTra->globalTransform * transformsToUpdate[i]->localTransform;
				transformsToUpdate[i]->globalTransformTransposed = transformsToUpdate[i]->globalTransform.Transposed();
				transformsToUpdate[i]->UpdatingMeshBounds();
			}
		}
	}

	transformsToUpdate.clear();
	updateTransform = false;
}

void Transform::UpdatingMeshBounds()
{
	MeshRenderer* mesh = dynamic_cast<MeshRenderer*>(GetOwner()->GetComponent(ComponentType::MESHRENDERER));
	if (mesh)
	{
		mesh->globalOBB = mesh->retMesh()->localAABB;
		mesh->globalOBB.Transform(globalTransform);
		mesh->globalAABB.SetNegativeInfinity();
		mesh->globalAABB.Enclose(mesh->globalOBB);
	}
}

Transform* Transform::GetRecursiveTransforms(Transform* node, std::vector<Transform*>& transforms)
{
	transforms.push_back(node);
	int numChildrens = node->GetOwner()->GetChildren().size();

	if (numChildrens != 0)
	{
		for (int i = 0; i < numChildrens; i++)
		{
			Transform* parentTrans = node->GetOwner()->GetChildren()[i]->transform;
			GetRecursiveTransforms(parentTrans, transforms);
		}
	}

	return nullptr;
}

void Transform::SetTransformMatrix(float3 position, Quat rotation, float3 localScale, Transform* parent)
{
	this->position = position;
	this->rotation = rotation;
	this->scale = localScale;

	eulerRotation = this->rotation.ToEulerXYZ() * RADTODEG;

	localTransform.SetRotatePart(float3x3::FromRS(rotation, scale));
	localTransform.SetCol3(3, position);

	if (parent != nullptr) {
		globalTransform = parent->globalTransform * localTransform;
		globalTransformTransposed = (parent->globalTransform * localTransform).Transposed();
	}
}