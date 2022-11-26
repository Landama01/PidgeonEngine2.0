#pragma once
#include "Module.h"
#include "glmath.h"
#include "Light.h"
#include "Primitive.h"

#include "Glew/include/glew.h"
#include "SDL\include\SDL_opengl.h"
#include "SDL\include\SDL.h"

class Texture;
class MeshRenderer;

#define MAX_LIGHTS 8
#define SQUARE_TEXTURE_W 256
#define SQUARE_TEXTURE_H 256

class Hardware {
public:
	std::string caps;
	std::string SDLVersion;

	uint CPUCount;
	uint CPUCache;
	float systemRAM;

	uint GPUVendor = 0;
	uint GPUDevice = 0;
	char GPUBrand[250] = "";
	float VRAMBudget = 0.f;
	float VRAMUsage = 0.f;
	float VRAMAvailable = 0.f;
	float VRAMReserved = 0.f;
};

class Renderer3D : public Module
{
public:
	Renderer3D(Application* app, bool start_enabled = true);
	~Renderer3D() {};

	bool Init()override;
	update_status PreUpdate(float dt)override;
	update_status PostUpdate(float dt)override;
	bool CleanUp();

	void GetCaps(std::string& caps);
	void OnResize(int width, int height);
	void OnGUI() override;

	void ReGenerateFrameBuffer(int w, int h);

	bool SaveConfig(JsonParser& node) const override;

	bool LoadConfig(JsonParser& node)override;

	Hardware GetHardware() { return hardware; };

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	bool vsync;
	bool wireframe = false;

	// Textures
	GLuint checkersTexture;
	GLubyte checkerImage[SQUARE_TEXTURE_W][SQUARE_TEXTURE_H][4];

	std::vector<MeshRenderer*> renderQueue;

	// FrameBuffer
	unsigned int framebuffer = 0;
	unsigned int texColorBuffer = 0;
	unsigned int rbo = 0;

	PrimitiveCube cube;
private:
	Hardware hardware;
	bool depthTest = true;
	bool cullFace = true;
	bool lighting = true;
	bool colorMaterial = true;
	bool texture2D = true;

	bool fog = false;
	bool fogLinear = true;
	bool fogExpo = false;
	float fogColor[3] = { 0.8f, 0.8f, 0.8f };
	float fogStart = 10.0f;
	float fogEnd = 40.0f;
	float fogDensity = 1.0f;


	PrimitiveSphere sphere;
	PrimitiveCylinder cylinder;
	PrimitivePyramid pyramid;	
};