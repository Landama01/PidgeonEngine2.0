#pragma once

#include<vector>
#include<string>

#include "Geometry/AABB.h"
#include "Geometry/OBB.h"

class Component;
enum class ComponentType;
class Transform;

class GameObject
{
public:
	GameObject(const char* name);
	virtual ~GameObject();

	void Update();

	template<class T> T* GetComponent()
	{
		T* component = nullptr;
		for (Component* c : components)
		{
			component = dynamic_cast<T*>(c);
			if (component)
				break;
		}
		return component;
	}

	Component* AddComponent(ComponentType type);
	Component* GetComponent(ComponentType type);

	void Enable();
	void Disable() { active = false; };

	bool GetShowChildrens() { return showChildrens; };
	void SetShowChildrens(bool active) { showChildrens = active; };

	std::vector<GameObject*> GetChildren() { return childrens; };
	void AddChildren(GameObject* child) { childrens.push_back(child); };

	GameObject* GetParent() { return parent; };
	void SetParent(GameObject* obj) { parent = obj; };
	std::vector<Component*> GetComponents() { return components; };

	bool IsRoot() { return (parent == nullptr) ? true : false; };
	void Destroy() { pendingToDelete = true; };
	bool GetPendingToDelete() { return pendingToDelete; };

	Transform* transform;

	bool active = true;
	bool isStatic = false;
	std::string name;

public:
	GameObject* parent;
	std::vector<GameObject*> childrens;
	std::vector<Component*> components;

	bool showChildrens = false;
	bool pendingToDelete = false;

	OBB globalOBB;
	AABB globalAABB;
};