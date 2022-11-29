#include"Application.h"
#include "SceneTab.h"

// Module
#include "Editor.h"
#include "Camera3D.h"

SceneTab::SceneTab() : Tab()
{
	name = "Scene";
}

void SceneTab::Draw()
{
	if (ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		// Check if some key/mouseButton are pressed
		if (ImGui::IsWindowHovered())
		{
			app->camera->CheckInputsMouse();
		}

		app->camera->CheckInputsKeyBoard();

		ImVec2 size = ImGui::GetContentRegionAvail();
		app->camera->cameraScene.RecalculateProjection(size.x / size.y);
		ImGui::Image((ImTextureID)app->camera->cameraScene.texColorBuffer, size, ImVec2(0, 1), ImVec2(1, 0));

		if (app->editor->GetGameObjectSelected() != nullptr)
			app->camera->DrawGuizmo(app->editor->GetGameObjectSelected());
	}
	ImGui::End();
}