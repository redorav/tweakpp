#pragma once

#include "imgui.h"

namespace tpp
{
	class ServerVariableManager;

	class UIVariableWindow
	{
	public:

		UIVariableWindow() {}

		void Draw(const tpp::ServerVariableManager& variableDescriptionTree, const char* title, bool* open = nullptr);

	private:

		float m_dummyFloat = 1.0f; // TODO Move to variable page
	};
}