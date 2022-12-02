#include "Application.h"
#include "GameTab.h"

#include "Renderer3D.h"
#include "Window.h"
#include "Camera3D.h"
#include "GO_Camera.h"

#include "Editor.h"

//#include "OpenGL.h"

GameTab::GameTab() : Tab()
{
	name = "Game";
}

GameTab::~GameTab()
{

}

void GameTab::Draw()
{

	if (ImGui::Begin(name.c_str(), NULL , ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{		
		if (app->editor->GO_camera != nullptr)
		{
			ImVec2 viewport = ImGui::GetContentRegionAvail();
			app->editor->GO_camera->aspectRatio = (viewport.x / viewport.y);
			app->editor->GO_camera->RecalculateProjection();
			ImGui::Image((ImTextureID)app->renderer3D->texColorBuffer, viewport, ImVec2(0, 1), ImVec2(1, 0));
		}					
	}
	ImGui::End();
}
