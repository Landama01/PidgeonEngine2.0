#include "ResourceTexture.h"
#include "Globals.h"
#include "FileSystem.h"
#include "TextureLoader.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"


Texture::Texture(std::string path, std::string name) : Resource (textureID, ResourceType::TEXTURE), path(path), name(name)
{
	SetAssetsPath(path.c_str());
	std::string libPath;
	if (path.find("/") != std::string::npos)
	{
		int pos = path.find_last_of("/");
		libPath = "Library/Textures" + path.substr(pos, path.find_last_of(".") - pos) + ".dds";
	}
	else
		libPath = "Library/Textures/" + path.substr(0, path.find_last_of(".")) + ".dds";
	SetLibraryPath(libPath.c_str());
}

Texture::~Texture()
{
}

bool Texture::LoadToMemory()
{
	LOG(LogType::L_WARNING, "Texture loaded to memory");
	char* buffer = nullptr;
	int size = FileSystem::LoadToBuffer(GetLibraryPath(), &buffer);

	textureID = TextureLoader::LoadToMemory(buffer, size, &texWidth, &texHeight);

	delete[] buffer;
	buffer = nullptr;

	return true;
}

bool Texture::UnloadFromMemory()
{
	if (textureID != 0)
		glDeleteTextures(1, &textureID);

	textureID = 0;
	texWidth = 0;
	texHeight = 0;

	return true;
}

void Texture::import(char* buffer, int size, const char* name)
{
	bool dds = TextureLoader::SaveDDS(buffer, size, name);
	if (dds) path = name;
}