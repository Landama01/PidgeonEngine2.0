#include "Application.h"
#include "MeshRenderer.h"
#include "Renderer3D.h"
#include "Mesh.h"
#include "GameObject.h"
#include "Transform.h"
#include "Material.h"
#include "ResourceMesh.h"
#include "Globals.h"
#include "Component.h"

#include "Glew/include/glew.h"
#include "glmath.h"
#include "Geometry/Sphere.h"

Mesh::Mesh(unsigned int uid) : Resource(uid, ResourceType::MESH)
{
}

Mesh::~Mesh()
{
	//Clear buffers (Should not happen on Mesh components)
	
	glDeleteBuffers(1, &id_index);
	glDeleteBuffers(1, &id_vertex);
	glDeleteBuffers(1, &id_texcoords);
	glDeleteBuffers(1, &id_normal);

	//Clear buffers
	UnloadFromMemory();
}

bool Mesh::LoadToMemory()
{
	// Vertex Buffer GL_ARRAY_BUFFER
	if (num_vertex != 0)
	{
		glGenBuffers(1, &id_vertex);
		glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 3, &vertex[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//Index Buffer GL_ELEMENT_ARRAY_BUFFER
	if (num_indices != 0)
	{
		glGenBuffers(1, &id_index);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_indices, &index[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// Normals Buffer GL_ARRAY_BUFFER
	if (num_normals != 0)
	{
		glGenBuffers(1, &id_normal);
		glBindBuffer(GL_ARRAY_BUFFER, id_normal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_normals * 3, &normals[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// TexCoords Buffer GL_ARRAY_BUFFER
	if (num_texcoords != 0)
	{
		glGenBuffers(1, &id_texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, id_texcoords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_texcoords * 2, &texcoords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//SetBounds();

	return true;
}

bool Mesh::UnloadFromMemory()
{
	//Clear buffers
	index.clear();
	vertex.clear();
	normals.clear();
	texcoords.clear();

	return true;
}

void Mesh::SetBounds()
{
	localAABB.SetNegativeInfinity();
	localAABB.Enclose(&(float3)vertex[0], num_vertex);

	Sphere sphere;
	sphere.r = 0.f;
	sphere.pos = localAABB.CenterPoint();
	sphere.Enclose(localAABB);

	radius = sphere.r;
	center = sphere.pos;

}

void Mesh::RenderMesh(GLuint textureID)
{
	if (textureID != -1)
		glBindTexture(GL_TEXTURE_2D, textureID);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	if (num_texcoords != 0)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, id_texcoords);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	}
	if (num_normals != 0)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, id_normal);
		glNormalPointer(GL_FLOAT, 0, NULL);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);

	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);

	if (textureID != -1)
		glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	if (num_normals != 0)
		glDisableClientState(GL_NORMAL_ARRAY);
	if (num_texcoords != 0)
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Mesh::RenderMeshDebug(bool* vertexNormals, bool* faceNormals)
{
	if (*vertexNormals == true)
	{
		float normalLenght = 0.05f;
		glPointSize(3.0f);
		glColor3f(1, 0, 0);
		glBegin(GL_POINTS);
		for (unsigned int i = 0; i < num_vertex * 3; i += 3)
		{
			glVertex3f(vertex[i], vertex[i + 1], vertex[i + 2]);
		}
		glEnd();
		glColor3f(0, 1, 0);
		glPointSize(1.0f);

		//Vertex normals
		glColor3f(0, 1, 0);
		glBegin(GL_LINES);
		for (unsigned int i = 0; i < num_normals * 3; i += 3)
		{
			glVertex3f(vertex[i], vertex[i + 1], vertex[i + 2]);
			glVertex3f(vertex[i] + normals[i] * normalLenght, vertex[i + 1] + normals[i + 1] * normalLenght, vertex[i + 2] + normals[i + 2] * normalLenght);
		}
		glEnd();
		glColor3f(1, 1, 1);
	}

	if (*faceNormals == true)
	{
		float normalLenght = 0.05f;
		//Face normals
		glColor3f(1, 0, 0);
		glBegin(GL_LINES);
		for (int i = 0; i < num_indices; i += 3)
		{
			float3 A = GetVectorFromIndex(&vertex[index[i] * 3]);
		
			float3 B = GetVectorFromIndex(&vertex[index[i + 1] * 3]);
		
			float3 C = GetVectorFromIndex(&vertex[index[i + 2] * 3]);
	
			float3 middle((A.x + B.x + C.x) / 3.f, (A.y + B.y + C.y) / 3.f, (A.z + B.z + C.z) / 3.f);
		
			float3 crossVec = Cross((B - A), (C - A));
			float3 normalDirection = crossVec.Normalized();

			//LOG("%f, %f, %f", middle.x, middle.y, middle.z);
			glVertex3f(middle.x, middle.y, middle.z);
			glVertex3f(middle.x + normalDirection.x * normalLenght, middle.y + normalDirection.y * normalLenght, middle.z + normalDirection.z * normalLenght);
		}
		glEnd();
		glPointSize(1.f);
		glColor3f(1, 1, 1);
	}
}

float3 Mesh::GetVectorFromIndex(float* startValue)
{
	float x = *startValue;
	++startValue;
	float y = *startValue;
	++startValue;
	float z = *startValue;

	return float3(x, y, z);
}