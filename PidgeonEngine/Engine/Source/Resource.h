#pragma once
#include<string>

enum class ResourceType {
	TEXTURE,
	MODEL,
	MESH,
	SCENE,
	SCRIPT,
	SHADER,
	MATERIAL,
	UNKNOWN
};

class Resource
{
public:
	

public:
	Resource(int uid, ResourceType type);
	virtual ~Resource();

	inline ResourceType GetType() const { return type; }
	inline int GetUID() const { return uid; }
	inline const char* GetAssetPath() const { return assetsFile.c_str(); }
	inline const char* GetLibraryPath() const { return libraryFile.c_str(); }

	void SetAssetsPath(const char*);
	void SetLibraryPath(const char*);

	inline unsigned int GetReferenceCount() const { return referenceCount; }
	inline void IncreaseReferenceCount() { referenceCount++; }
	inline void DecreaseReferenceCount() { referenceCount--; }

	virtual bool LoadToMemory() { return false; }
	virtual bool UnloadFromMemory() { return false; }

private:

	int uid;
	std::string assetsFile;
	std::string libraryFile;
	ResourceType type = ResourceType::UNKNOWN;
	unsigned int referenceCount;
};