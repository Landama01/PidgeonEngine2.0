#include "GameObject.h"
#include "Component.h"

#include "Transform.h"
#include "MeshRenderer.h"
#include "Material.h"

GameObject::GameObject(const char* name) : parent(nullptr), name(name)
{
	transform = dynamic_cast<Transform*>(AddComponent(ComponentType::TRANSFORM));
}

GameObject::~GameObject()
{
	for (size_t i = 0; i < components.size(); i++)
	{
		delete components[i];
		components[i] = nullptr;
	}
	components.clear();

	for (size_t i = 0; i < children.size(); i++)
	{
		delete children[i];
		children[i] = nullptr;
	}
	children.clear();
}

void GameObject::Enable()
{
	active = true;

	if (parent != nullptr)
		parent->Enable();
}

void GameObject::Update()
{
	// Call all components Updates
	for (size_t i = 0; i < components.size(); i++)
	{
		components[i]->Update();
	}
}
// Add component by Type
Component* GameObject::AddComponent(ComponentType type)
{
	assert(type != ComponentType::UNKNOW, "Can't create a UNKNOW component");
	Component* ret = nullptr;

	switch (type)
	{
	case ComponentType::TRANSFORM:
		ret = new Transform(this);
		break;
	case ComponentType::MESHRENDERER:
		ret = new MeshRenderer(this);
		break;
	case ComponentType::MATERIAL:
		ret = new Material(this);
		break;
	}

	if (ret != nullptr)
	{
		ret->SetType(type);
		components.push_back(ret);
	}

	return ret;
}
// Get component by Type, because an object can only have 1 component of 1 type
Component* GameObject::GetComponent(ComponentType type)
{
	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == type)
			return components[i];
	}

	return nullptr;
}