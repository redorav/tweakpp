#include "tppUIVariableWindow.h"

void tpp::UIVariableWindow::Draw(const tpp::ServerVariableManager& variableManager, const char* title, bool* open)
{
	static float myFloat = 0.0f;

	ImGui::SliderFloat("My Float", &myFloat, 0.0f, 10.0f);

	ImGui::DragFloat("Drag Float", &myFloat, 0.001f, 0.0f, 10.0f);

	ImGui::InputFloat("Input Float", &myFloat, 0.001f, 0.01f);
}