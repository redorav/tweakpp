#pragma once

#include "imgui.h"

namespace tpp
{
	class ServerVariableManager;

	class UIVariableTree
	{
	public:

		UIVariableTree()
		{

		}

		void Draw(const tpp::ServerVariableManager& variableManager, const char* title, bool* open = nullptr);

	private:

	};
}