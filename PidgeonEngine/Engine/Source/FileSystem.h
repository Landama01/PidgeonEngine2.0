#ifndef __FileSystem_H__
#define __FileSystem_H__

#include "PhysFS/include/physfs.h"

#include <vector>
#include <string>

struct SDL_RWops;
int close_sdl_rwops(SDL_RWops *rw);

enum class ImportType {
	NOTYPE = -1,
	MESH,
	TEXTURE,
};

namespace StringLogic {

	std::string FileNameFromPath(const char* _path);
	std::string GlobalToLocalPath(const char* _globalPath);
}

namespace FileSystem 
{
	ImportType GetTypeFromPath(const char* path);

	void FSInit();
	void FSDeInit();
	void CreateLibraryDirectories();

	bool AddPath(const char* path_or_zip);
	bool Exists(const char* file);
	bool CreateDir(const char* dir);
	bool IsDirectory(const char* file);

	std::string NormalizePath(const char* full_path);
	std::string UnNormalizePath(const char* full_path);

	unsigned int LoadToBuffer(const char* file, char** buffer);
	unsigned int Save(const char* file, char* buffer, unsigned int size, bool append);

	bool Remove(const char* file);
	void GetFileName(const char* file, std::string& fileName, bool extension);

	inline const char* GetBasePath() { return PHYSFS_getBaseDir(); }
	inline const char* GetWritePath() { return PHYSFS_getWriteDir(); }
	const char* GetReadPaths();

	void OnGui();
};

#endif // __FileSystem_H__
