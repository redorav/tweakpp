#pragma once

#include "imgui.h"

namespace tpp
{
	class ServerVariableManager;

	class UIVariableGroupWindow
	{
	public:

		UIVariableGroupWindow()
		{

		}

		void Draw(const tpp::ServerVariableManager& variableManager, const char* title, bool* open = nullptr);

	private:

	};
}