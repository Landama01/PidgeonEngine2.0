#pragma once
#include"Resource.h"
#include"Color.h"

typedef unsigned int GLuint;

class Texture : public Resource
{
public:
	Texture(std::string path, std::string name = "");
	~Texture();

	bool LoadToMemory() override;
	bool UnloadFromMemory() override;
	void import(char* buffer, int size, const char* name);

public:
	int texWidth;
	int texHeight;

	GLuint textureID;
	Color color;

	std::string path;
	std::string name;
};
