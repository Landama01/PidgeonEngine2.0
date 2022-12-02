#include "Application.h"
#include "FileSystem.h"
#include "Globals.h"

#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"
#include "MeshLoader.h"

// File System Init
void FileSystem::FSInit()
{
	LOG(LogType::L_NORMAL, "DevIL Init");
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	MeshLoader::EnableDebugMode();

	char* base_path = SDL_GetBasePath();
	PHYSFS_init(base_path);
	SDL_free(base_path);

	if (PHYSFS_setWriteDir(base_path) == 0)
		LOG(LogType::L_NORMAL, "File System error while creating write dir: %s\n", PHYSFS_getLastError());

	std::string assetPath = GetBasePath();
	std::string path = NormalizePath(assetPath.c_str());
	FileSystem::AddPath(assetPath.c_str());

	assetPath = GetBasePath();
	assetPath = NormalizePath(assetPath.c_str());
	assetPath += ASSETS_FOLDER;	
	FileSystem::AddPath(assetPath.c_str());

	assetPath = GetBasePath();
	assetPath = NormalizePath(assetPath.c_str());
	FileSystem::AddPath(assetPath.c_str());

	LOG(LogType::L_NORMAL, "FileSystem Operations base is [%s] plus:", GetBasePath());
	LOG(LogType::L_NORMAL, GetReadPaths());

	FileSystem::CreateLibraryDirectories();
}

std::string StringLogic::FileNameFromPath(const char* _path)
{
	std::string fileName(_path);

	fileName = fileName.substr(fileName.find_last_of("/\\") + 1);
	fileName = fileName.substr(0, fileName.find_last_of('.'));

	return fileName;
}

std::string StringLogic::GlobalToLocalPath(const char* _globalPath)
{
	std::string localPath = FileSystem::NormalizePath(_globalPath);

	size_t pos = 0;
	pos = localPath.find(ASSETS_FOLDER);
	if (pos != std::string::npos)
	{
		localPath = localPath.substr(pos);
		if (localPath.c_str() != "")
			return localPath;
	}

	localPath.clear();
	return localPath;
}

ImportType FileSystem::GetTypeFromPath(const char* path)
{
	std::string ext(path);
	ext = ext.substr(ext.find_last_of('.') + 1);

	for (int i = 0; i < ext.length(); i++)
	{
		ext[i] = std::tolower(ext[i]);
	}

	if (ext == "fbx" || ext == "dae")
		return ImportType::MESH;
	if (ext == "tga" || ext == "png" || ext == "jpg" || ext == "dds")
		return ImportType::TEXTURE;

	return ImportType::NOTYPE;
}

void FileSystem::FSDeInit()
{
	PHYSFS_deinit();
	MeshLoader::DisableDebugMode();
}

void FileSystem::CreateLibraryDirectories()
{
	CreateDir(LIBRARY_FOLDER);
	CreateDir(MESHES_FOLDER);
	CreateDir(TEXTURES_FOLDER);
	CreateDir(MATERIALS_FOLDER);
}
bool FileSystem::CreateDir(const char* dir)
{
	if (IsDirectory(dir) == false)
	{
		PHYSFS_mkdir(dir);
		return true;
	}
	return true;
}

bool FileSystem::Exists(const char* file)
{
	return PHYSFS_exists(file) != 0;
}


bool FileSystem::AddPath(const char* path_or_zip)
{
	bool ret = false;

	if (PHYSFS_mount(path_or_zip, nullptr, 1) == 0)
	{
		LOG(LogType::L_ERROR, "File System error while adding a path or zip: %s\n", PHYSFS_getLastError());
	}
	else
		ret = true;

	return ret;
}

const char* FileSystem::GetReadPaths()
{
	static char paths[512];

	paths[0] = '\0';

	char** path;
	for (path = PHYSFS_getSearchPath(); *path != nullptr; path++)
	{
		strcat_s(paths, 512, *path);
		strcat_s(paths, 512, "\n");
	}

	return paths;
}

bool FileSystem::IsDirectory(const char* file)
{
	return PHYSFS_isDirectory(file) != 0;
}

std::string FileSystem::NormalizePath(const char* full_path)
{
	std::string newPath(full_path);
	for (int i = 0; i < newPath.size(); ++i)
	{
		if (newPath[i] == '\\')
			newPath[i] = '/';
	}
	return newPath;
}
std::string FileSystem::UnNormalizePath(const char* full_path)
{
	std::string newPath(full_path);
	for (int i = 0; i < newPath.size(); ++i)
	{
		if (newPath[i] == '/')
			newPath[i] = '\\';
	}
	return newPath;
}

uint FileSystem::LoadToBuffer(const char* file, char** buffer)
{
	uint ret = 0;

	PHYSFS_file* fs_file = PHYSFS_openRead(file);

	if (fs_file != nullptr)
	{
		PHYSFS_sint64 size = PHYSFS_fileLength(fs_file);

		if (size > 0)
		{
			*buffer = new char[size + 1];
			uint readed = (uint)PHYSFS_read(fs_file, *buffer, 1, size);
			if (readed != size)
			{
				LOG(LogType::L_ERROR, "File System error while reading from file %s: %s\n", file, PHYSFS_getLastError());
				RELEASE_ARRAY(buffer);
			}
			else
			{
				ret = readed;
				(*buffer)[size] = '\0';
			}
		}

		if (PHYSFS_close(fs_file) == 0)
			LOG(LogType::L_ERROR, "File System error while closing file %s: %s\n", file, PHYSFS_getLastError());
	}
	else
		LOG(LogType::L_ERROR, "File System error while opening file %s: %s\n", file, PHYSFS_getLastError());

	return ret;
}

unsigned int FileSystem::Save(const char* file, char* buffer, unsigned int size, bool append)
{
	uint objCount = 0;

	std::string fileName;
	GetFileName(file, fileName, true);

	bool exists = Exists(file);

	PHYSFS_file* filehandle = nullptr;
	if (append)
		filehandle = PHYSFS_openAppend(file);
	else
		filehandle = PHYSFS_openWrite(file);

	if (filehandle != nullptr)
	{
		objCount = PHYSFS_writeBytes(filehandle, (const void*)buffer, size);

		if (objCount == size)
		{
			if (exists)
			{
				if (append)
				{
					LOG(LogType::L_NORMAL, "FILE SYSTEM: Append %u bytes to file '%s'", objCount, fileName.data());
				}
				else
					LOG(LogType::L_NORMAL, "FILE SYSTEM: File '%s' overwritten with %u bytes", fileName.data(), objCount);
			}
			else
				LOG(LogType::L_NORMAL, "FILE SYSTEM: New file '%s' created with %u bytes", fileName.data(), objCount);
		}
		else
			LOG(LogType::L_ERROR, "FILE SYSTEM: Could not write to file '%s'. ERROR: %s", fileName.data(), PHYSFS_getLastError());

		if (PHYSFS_close(filehandle) == 0)
			LOG(LogType::L_ERROR, "FILE SYSTEM: Could not close file '%s'. ERROR: %s", fileName.data(), PHYSFS_getLastError());
	}
	else
		LOG(LogType::L_ERROR, "FILE SYSTEM: Could not open file '%s' to write. ERROR: %s", fileName.data(), PHYSFS_getLastError());

	return objCount;
}

bool FileSystem::Remove(const char* file)
{
	bool ret = false;

	if (file != nullptr)
	{
		if (PHYSFS_delete(file) != 0)
		{
			LOG(LogType::L_NORMAL, "File deleted: [%s]", file);
			ret = true;
		}
		else
			LOG(LogType::L_ERROR, "File System error while trying to delete [%s]: %s", file, PHYSFS_getLastError());
	}

	return ret;
}

void FileSystem::GetFileName(const char* file, std::string& fileName, bool extension)
{
	fileName = file;

	uint found = fileName.find_last_of("\\");
	if (found != std::string::npos)
		fileName = fileName.substr(found + 1, fileName.size());

	found = fileName.find_last_of("//");
	if (found != std::string::npos)
		fileName = fileName.substr(found + 1, fileName.size());

	if (!extension)
	{
		found = fileName.find_last_of(".");
		if (found != std::string::npos)
			fileName = fileName.substr(0, found);
	}
}

void FileSystem::OnGui()
{
	if (ImGui::CollapsingHeader("FileSystem"))
	{
		IMGUI_PRINT("Base Path: ", GetBasePath());
		IMGUI_PRINT("Read Paths: ", GetReadPaths());
		IMGUI_PRINT("Write Path: ", GetWritePath());
	}
}