#pragma once
#include "Component.h"
#include "Geometry/AABB.h"
#include "Geometry/OBB.h"

class Mesh;

class MeshRenderer : public Component
{
public:
	MeshRenderer(GameObject* obj);
	virtual ~MeshRenderer();

	void Update() override;
	void RenderMesh();
	void SetBounds();
	void OnEditor() override;

	Mesh* retMesh() { return mesh; };

	Mesh* mesh;
	bool faceNormals = false;
	bool vertexNormals = false;

	AABB globalAABB;
	OBB globalOBB;
};