#include "FBXLoader.h"
#include "Globals.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"

#include "mmgr/mmgr.h"

FBXLoader::FBXLoader()
{
}

FBXLoader::~FBXLoader()
{
}

void FBXLoader::LoadMesh(const char* path)
{
	//const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	//if (scene != nullptr && scene->HasMeshes())
	//{
	//	for (int i = 0; i < scene->mNumMeshes; ++i)
	//	{
	//		aiMesh* mesh = scene->mMeshes[i];
	//		ourMesh.numVertex = mesh->mNumVertices;
	//		ourMesh.vertices = new float3[mesh->mNumVertices];
	//		memcpy(ourMesh.vertices, mesh->mVertices, sizeof(float3) * mesh->mNumVertices);

	//		LOG("New mesh with %d vertices", mesh->mNumVertices);

	//		if (mesh->HasFaces())
	//		{
	//			ourMesh.numIndex = mesh->mNumFaces * 3;
	//			ourMesh.indices = new uint[ourMesh.numIndex];
	//			
	//			for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
	//			{
	//				aiFace& face = mesh->mFaces[j];
	//				if (face.mNumIndices != 3)
	//					LOG("WARNING, geometry face with != 3 indices!");
	//				else
	//					memcpy(&ourMesh.indices[j * 3], face.mIndices, 3 * sizeof(uint));
	//			}
	//		}

	//		if (mesh->HasNormals())
	//		{
	//			ourMesh.numNormals = mesh->mNumVertices;
	//			ourMesh.normals = new float3[ourMesh.numNormals];
	//			memcpy(ourMesh.normals, mesh->mNormals, sizeof(float3) * ourMesh.numNormals);
	//		}
	//	}
	//	// Use scene->mNumMeshes to iterate on scene->mMeshes array
	//	aiReleaseImport(scene);
	//}
	//else
	//	LOG("Error loading scene % s", path);
}