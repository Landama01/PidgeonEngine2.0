#include "Application.h"
#include "ResourceManager.h"

//Importers
#include "FileSystem.h"
#include "TextureLoader.h"
#include "MeshLoader.h"
#include "ModelImporter.h"

//Resources
#include "ResourceTexture.h"
#include "ResourceMesh.h"

//Tabs
#include "Inspector.h" 

//Components
#include "Material.h"

ResourceManager::ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "ResourceManager";
}

ResourceManager::~ResourceManager()
{
}

bool ResourceManager::Init()
{
	FileSystem::FSInit();
	return true;
}

bool ResourceManager::Start()
{	
	logo = new Texture("logo.png", "logo");
	logo->LoadToMemory();

	return true;
}

bool ResourceManager::CleanUp()
{
	RELEASE(logo);
	FileSystem::FSDeInit();
	return true;
}

void ResourceManager::ImportFile(const char* assetsFile)
{
	std::string normalizedPath = FileSystem::NormalizePath(assetsFile);

	std::string output = "";

	char* buffer = nullptr;
	uint size = FileSystem::LoadToBuffer(normalizedPath.c_str(), &buffer);

	if (buffer != nullptr && size != 0)
	{
		switch (FileSystem::GetTypeFromPath(assetsFile))
		{
		case ImportType::NOTYPE:
		{
			LOG(LogType::L_ERROR, "File extension not supported yet");
			break;
		}
		case ImportType::MESH:
		{
			ModelImporter::Import(normalizedPath.c_str(), buffer, size, app->scene->root);
			break;
		}
		case ImportType::TEXTURE:
		{			
			Texture* material = new Texture(normalizedPath.c_str());

			material->import(buffer, size, material->GetLibraryPath());
			material->LoadToMemory();

			Inspector* inspector = static_cast<Inspector*>(app->editor->GetTab(TabType::INSPECTOR));
			if (inspector && inspector->gameObjectSelected) {
				Material* mat = static_cast<Material*>(inspector->gameObjectSelected->GetComponent(ComponentType::MATERIAL));
				if (mat)
				{
					mat->matTexture = material;
				}
				else {
					Material* mat = static_cast<Material*>(inspector->gameObjectSelected->AddComponent(ComponentType::MATERIAL));
					mat->matTexture = material;
				}
			}
			break;
		}

		}
		RELEASE_ARRAY(buffer);
	}
}
