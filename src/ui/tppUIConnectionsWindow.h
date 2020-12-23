#pragma once

#include "imgui.h"

namespace tpp
{
	class UIConnectionsWindow
	{
	public:

		UIConnectionsWindow() {}

		void Draw(const char* title, bool* open = nullptr);

	private:

		float m_dummyFloat = 1.0f; // TODO Move to variable page
	};
}