#pragma once

#include "imgui.h"

#include <vector>

#include <string>

namespace tpp
{
	class UILog
	{
	public:

		UILog();

		void Log(const char* format...);

		void Clear();

		void Draw(const char* title, bool* open = nullptr);

	private:

		ImGuiTextFilter m_filter;

		// TODO Make more efficient
		std::vector<std::string> m_logBuffer;

		ImGuiWindowFlags m_windowFlags = 0;

		bool m_autoScroll = true;
	};
}