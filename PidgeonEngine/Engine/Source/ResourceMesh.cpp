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
	
	glDeleteBuffers(1, &id_index);
	glDeleteBuffers(1, &id_vertex);
	glDeleteBuffers(1, &id_texcoords);
	glDeleteBuffers(1, &id_normal);

	UnloadFromMemory();
}

bool Mesh::LoadToMemory()
{
	if (num_indices != 0)
	{
		glGenBuffers(1, &id_index);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_indices, &index[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	if (num_vertex != 0)
	{
		glGenBuffers(1, &id_vertex);
		glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 3, &vertex[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}	

	if (num_normals != 0)
	{
		glGenBuffers(1, &id_normal);
		glBindBuffer(GL_ARRAY_BUFFER, id_normal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_normals * 3, &normals[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (num_texcoords != 0)
	{
		glGenBuffers(1, &id_texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, id_texcoords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_texcoords * 2, &texcoords[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	return true;
}

bool Mesh::UnloadFromMemory()
{
	index.clear();
	vertex.clear();
	normals.clear();
	texcoords.clear();

	return true;
}

void Mesh::SetBounds()
{
	localAABB.SetNegativeInfinity();

	std::vector<float3> vertices;
	float3 StroePoints;
	for (int i = 0; i < vertex.size(); i += 3)
	{
		StroePoints = float3(vertex[i], vertex[i + 1], vertex[i + 2]);
		vertices.push_back(StroePoints);
	}

	localAABB.Enclose(&vertices[0], vertices.size());

	Sphere sphere;
	sphere.r = 0.f;
	sphere.pos = localAABB.CenterPoint();
	sphere.Enclose(localAABB);

	radius = sphere.r;
	center = sphere.pos;

	vertices.clear();
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
	float normalLenght = 0.05f;

	if (*faceNormals == true)
	{		
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

			glVertex3f(middle.x, middle.y, middle.z);
			glVertex3f(middle.x + normalDirection.x * normalLenght, middle.y + normalDirection.y * normalLenght, middle.z + normalDirection.z * normalLenght);
		}
		glEnd();
		glPointSize(1.f);
		glColor3f(1, 1, 1);
	}

	if (*vertexNormals == true)
	{		
		for (int x = 0; x <=1; x++)
		{						
			(x == 0) ? glPointSize(3.0f), glBegin(GL_POINTS) : glBegin(GL_LINES);
			if (x == 0)
			{
				glColor3f(1, 0, 0);
				for (unsigned int i = 0; i < num_vertex * 3; i += 3)
				{
					glVertex3f(vertex[i], vertex[i + 1], vertex[i + 2]);
				}				
			}
			if (x == 1)
			{
				glColor3f(0, 1, 0);
				for (unsigned int i = 0; i < num_normals * 3; i += 3)
				{
					glVertex3f(vertex[i], vertex[i + 1], vertex[i + 2]);
					glVertex3f(vertex[i] + normals[i] * normalLenght, vertex[i + 1] + normals[i + 1] * normalLenght, vertex[i + 2] + normals[i + 2] * normalLenght);
				}

				glColor3f(1, 1, 1);
				if (x == 0) glPointSize(1.0f);
				glEnd();
				
			}			
		}		
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