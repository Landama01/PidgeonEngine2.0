#pragma once

typedef unsigned int GLuint;
class Resource;

namespace TextureLoader
{
	GLuint LoadToMemory(char* buffer, int size, int* w = nullptr, int* h = nullptr);
	void SaveDDS(char* buffer, int size, const char* fileName);

	void Import(char* buffer, int bSize, Resource* res);
};
