#include "Globals.h"
#include "Application.h"
#include "FileSystem.h"


#include "Assimp/include/cfileio.h"
#include "Assimp/include/types.h"


//#include "Leaks.h"

#pragma comment( lib, "../Engine/Source/External/PhysFS/libx86/physfs.lib" )

using namespace std;

FileSystem::FileSystem(const char* game_path) : name("File System")
{
	// needs to be created before Init so other modules can use it
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(base_path);
	SDL_free(base_path);

	// workaround VS string directory mess
	AddPath(".");

	if(0&&game_path != nullptr)
		AddPath(game_path);

	// Dump list of paths
	LOG(LogType::L_NORMAL,"FileSystem Operations base is [%s] plus:", GetBasePath());
	LOG(LogType::L_NORMAL, GetReadPaths());

	// enable us to write in the game's dir area
	if(PHYSFS_setWriteDir(".") == 0)
		LOG(LogType::L_ERROR, "File System error while creating write dir: %s\n", PHYSFS_getLastError());

	// Make sure standard paths exist
	const char* dirs[] = {
		SETTINGS_FOLDER, ASSETS_FOLDER
	};

	for (uint i = 0; i < sizeof(dirs)/sizeof(const char*); ++i)
	{
		if (PHYSFS_exists(dirs[i]) == 0)
			PHYSFS_mkdir(dirs[i]);
	}

	// Generate IO interfaces
	CreateAssimpIO();
}

// Destructor
FileSystem::~FileSystem()
{
	RELEASE(assimpIO);
	PHYSFS_deinit();
}

// Called before render is available
bool FileSystem::Init(JsonParser& config)
{
	LOG(LogType::L_NORMAL, "Loading File System");
	bool ret = true;

	// Ask SDL for a write dir
	char* write_path = SDL_GetPrefPath(app->GetOrganizationName(), app->GetAppName());

	// Trun this on while in game mode
	//if(PHYSFS_setWriteDir(write_path) == 0)
		//LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError());


	SDL_free(write_path);

	return ret;
}

// Called before quitting
bool FileSystem::CleanUp()
{
	//LOG("Freeing File System subsystem");

	return true;
}

// Add a new zip file or folder
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

// Flatten filenames to always use lowercase and / as folder separator
char normalize_char(char c)
{
	if (c == '\\')
		return '/';
	return tolower(c);
}


// Read a whole file and put it in a new buffer
uint FileSystem::Load(const char* file, char** buffer) const
{
	uint ret = 0;

	PHYSFS_file* fs_file = PHYSFS_openRead(file);

	if(fs_file != nullptr)
	{
		PHYSFS_sint32 size = (PHYSFS_sint32) PHYSFS_fileLength(fs_file);

		if(size > 0)
		{
			*buffer = new char[size];
			uint readed = (uint) PHYSFS_read(fs_file, *buffer, 1, size);
			if(readed != size)
			{
				LOG(LogType::L_ERROR, "File System error while reading from file %s: %s\n", file, PHYSFS_getLastError());
				RELEASE(buffer);
			}
			else
				ret = readed;
		}

		if(PHYSFS_close(fs_file) == 0)
			LOG(LogType::L_ERROR, "File System error while closing file %s: %s\n", file, PHYSFS_getLastError());
	}
	else
		LOG(LogType::L_ERROR, "File System error while opening file %s: %s\n", file, PHYSFS_getLastError());

	return ret;
}

int close_sdl_rwops(SDL_RWops *rw)
{
	RELEASE_ARRAY(rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;
}

// Save a whole buffer to disk
uint FileSystem::Save(const char* file, const void* buffer, unsigned int size, bool append)
{
	unsigned int ret = 0;

	bool overWrite = PHYSFS_exists(file) != 0;
	PHYSFS_file* fsFile = (append) ? PHYSFS_openAppend(file) : PHYSFS_openWrite(file);

	if (fsFile != nullptr)
	{
		uint written = (uint)PHYSFS_write(fsFile, (const void*)buffer, 1, size);
		if (written != size)
		{
			LOG(LogType::L_ERROR, "File System error while writing to file %s: %s", file, PHYSFS_getLastError());
		}
		else
		{
			if (append == true)
			{
				LOG(LogType::L_NORMAL, "Added %u data to [%s%s]", size, PHYSFS_getWriteDir(), file); 
			}
			else if (overWrite == false) {
				LOG(LogType::L_NORMAL, "New file created [%s%s] of %u bytes", PHYSFS_getWriteDir(), file, size);
			}
			ret = written;
		}

		if (PHYSFS_close(fsFile) == 0)
			LOG(LogType::L_ERROR, "File System error while closing file %s: %s", file, PHYSFS_getLastError());
	}
	else
		LOG(LogType::L_ERROR,"File System error while opening file %s: %s", file, PHYSFS_getLastError());

	return ret;
}


const char * FileSystem::GetReadPaths() const
{
	static char paths[512];

	paths[0] = '\0';

	char **path;
	for (path = PHYSFS_getSearchPath(); *path != nullptr; path++)
	{
		strcat_s(paths, 512, *path);
		strcat_s(paths, 512, "\n");
	}

	return paths;
}

// -----------------------------------------------------
// ASSIMP IO
// -----------------------------------------------------

size_t AssimpWrite(aiFile* file, const char* data, size_t size, size_t chunks)
{
	PHYSFS_sint64 ret = PHYSFS_write((PHYSFS_File*)file->UserData, (void*)data, size, chunks);
	if(ret == -1)
		LOG(LogType::L_ERROR, "File System error while WRITE via assimp: %s", PHYSFS_getLastError());

	return (size_t) ret;
}

size_t AssimpRead(aiFile* file, char* data, size_t size, size_t chunks)
{
	PHYSFS_sint64 ret = PHYSFS_read((PHYSFS_File*)file->UserData, (void*)data, size, chunks);
	if(ret == -1)
		LOG(LogType::L_ERROR, "File System error while READ via assimp: %s", PHYSFS_getLastError());

	return (size_t) ret;
}

size_t AssimpTell(aiFile* file)
{
	PHYSFS_sint64 ret = PHYSFS_tell((PHYSFS_File*)file->UserData);
	if(ret == -1)
		LOG(LogType::L_ERROR, "File System error while TELL via assimp: %s", PHYSFS_getLastError());

	return (size_t) ret;
}

size_t AssimpSize(aiFile* file)
{
	PHYSFS_sint64 ret = PHYSFS_fileLength((PHYSFS_File*)file->UserData);
	if(ret == -1)
		LOG(LogType::L_ERROR, "File System error while SIZE via assimp: %s", PHYSFS_getLastError());

	return (size_t) ret;
}

void AssimpFlush(aiFile* file)
{
	if(PHYSFS_flush((PHYSFS_File*)file->UserData) == 0)
		LOG(LogType::L_ERROR, "File System error while FLUSH via assimp: %s", PHYSFS_getLastError());
}

aiReturn AssimpSeek(aiFile* file, size_t pos, aiOrigin from)
{
	int res = 0;

	switch (from)
	{
	case aiOrigin_SET:
		res = PHYSFS_seek((PHYSFS_File*)file->UserData, pos);
		break;
	case aiOrigin_CUR:
		res = PHYSFS_seek((PHYSFS_File*)file->UserData, PHYSFS_tell((PHYSFS_File*)file->UserData) + pos);
		break;
	case aiOrigin_END:
		res = PHYSFS_seek((PHYSFS_File*)file->UserData, PHYSFS_fileLength((PHYSFS_File*)file->UserData) + pos);
		break;
	}

	if(res == 0)
		LOG(LogType::L_ERROR, "File System error while SEEK via assimp: %s", PHYSFS_getLastError());

	return (res != 0) ? aiReturn_SUCCESS : aiReturn_FAILURE;
}

aiFile* AssimpOpen(aiFileIO* io, const char* name, const char* format)
{
	static aiFile file;

	file.UserData = (char*) PHYSFS_openRead(name);
	file.ReadProc = AssimpRead;
	file.WriteProc = AssimpWrite;
	file.TellProc = AssimpTell;
	file.FileSizeProc = AssimpSize;
	file.FlushProc= AssimpFlush;
	file.SeekProc = AssimpSeek;

	return &file;
}

void AssimpClose(aiFileIO* io, aiFile* file)
{
	if (PHYSFS_close((PHYSFS_File*)file->UserData) == 0)
		LOG(LogType::L_ERROR, "File System error while CLOSE via assimp: %s", PHYSFS_getLastError());
}

void FileSystem::CreateAssimpIO()
{
	RELEASE(assimpIO);

	assimpIO = new aiFileIO;
	assimpIO->OpenProc = AssimpOpen;
	assimpIO->CloseProc = AssimpClose;
}

aiFileIO* FileSystem::GetAssimpIO()
{
	return assimpIO;
}




