#pragma once

#include "tppUIBackend.h"

#include "imgui.h"

int tpp::UIBackend::WindowWidth;

int tpp::UIBackend::WindowHeight;

bool tpp::UIBackend::Initialize(const UIInitializeParams& params)
{
	WindowWidth = params.windowWidth;
	WindowHeight = params.windowHeight;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();

	InitializePlatform(params);

	return true;
}

bool tpp::UIBackend::Shutdown()
{
	ShutdownPlatform();

	ImGui::DestroyContext();

	return true;
}

bool tpp::UIBackend::Render()
{
	ImGui::Render();

	RenderPlatform();

	return true;
}

tpp::UIBackendResult tpp::UIBackend::PrepareNewFrame()
{
	tpp::UIBackendResult result = PrepareNewFramePlatform();

	if (result == tpp::UIBackendResult::Ok)
	{
		ImGui::NewFrame();
	}

	return result;
}
