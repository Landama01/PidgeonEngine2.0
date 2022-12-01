#include "Application.h"
#include "GameTab.h"

#include "Renderer3D.h"
#include "Window.h"
#include "Camera3D.h"

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
		
		ImVec2 texOriginalSize = ImVec2(app->window->GetWindowWidth(), app->window->GetWindowHeight());
		ImVec2 e = ImGui::GetWindowSize();
		
		ImVec2 startPoint = ImVec2((texOriginalSize.x / 2) - (e.x / 2), (texOriginalSize.y / 2) + (e.y / 2));
		ImVec2 endPoint = ImVec2((texOriginalSize.x / 2) + (e.x / 2), (texOriginalSize.y / 2) - (e.y / 2));

		ImVec2 uv0 = ImVec2(startPoint.x / texOriginalSize.x, startPoint.y / texOriginalSize.y);

		ImVec2 uv1 = ImVec2(endPoint.x / texOriginalSize.x, endPoint.y / texOriginalSize.y);

		ImGui::Image((ImTextureID)app->renderer3D->texColorBuffer, e, uv0, uv1);
					
	}
	ImGui::End();
}
