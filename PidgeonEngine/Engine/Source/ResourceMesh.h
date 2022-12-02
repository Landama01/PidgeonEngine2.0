#pragma once
#include "Resource.h"
#include <vector>
#include "Geometry/AABB.h"
#include "Math/float3.h"

typedef unsigned int uint;
typedef unsigned int GLuint;
class vec3;

class Mesh : public Resource 
{
public:
	Mesh(unsigned int _uid);
	~Mesh();

	bool LoadToMemory() override;
	bool UnloadFromMemory() override;

	void SetBounds();

	void RenderMesh(GLuint textureID = -1);
	void RenderMeshDebug(bool* vertexNormals, bool* faceNormals);
	float3 GetVectorFromIndex(float* startValue);

	void CleanUp();
public:
	// Buffers ID
	uint id_index = 0;
	uint id_vertex = 0;
	uint id_texcoords = 0;
	uint id_normal = 0;
	// Counts
	uint num_indices = 0;
	uint num_vertex = 0;
	uint num_texcoords = 0;
	uint num_normals = 0;
	// Vectors
	std::vector<uint> index;
	std::vector<float> vertex;
	std::vector<float> texcoords;
	std::vector<float> normals;

private:
	float3 center = float3::zero;
	float radius;

	AABB localAABB;
};