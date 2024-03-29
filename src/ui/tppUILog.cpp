#include "tppUILog.h"

tpp::UILog::UILog()
{
	m_windowFlags |= ImGuiWindowFlags_NoCollapse;
}

void tpp::UILog::Log(const char* format...)
{
	va_list args;
	va_start(args, format);

	// TODO Optimize

	char buffer[2048];
	vsnprintf(buffer, sizeof(buffer), format, args);

	m_logBuffer.push_back(buffer);

	va_end(args);
}

void tpp::UILog::Clear()
{
	m_logBuffer.clear();
}

void tpp::UILog::Draw(const char* title, bool* p_open)
{
	if (!ImGui::Begin(title, p_open, m_windowFlags))
	{
		ImGui::End();
		return;
	}

	if (ImGui::Button("Clear"))
	{
		Clear();
	}

	ImGui::SameLine();

	if (ImGui::Button("Copy"))
	{
		std::string fullString;

		for (size_t i = 0; i < m_logBuffer.size(); ++i)
		{
			fullString += m_logBuffer[i];
		}

		ImGui::SetClipboardText(fullString.c_str());
	}

	ImGui::SameLine();
	ImGui::Checkbox("Auto-scroll", &m_autoScroll);

	ImGui::SameLine();
	m_filter.Draw("Filter", -60.0f);

	ImGui::Separator();

	ImGuiWindowFlags scrollingSubWindowFlags = 0;
	scrollingSubWindowFlags |= ImGuiWindowFlags_HorizontalScrollbar;

	ImGui::BeginChild("scrolling", ImVec2(0, 0), false, scrollingSubWindowFlags);
	{
		for (size_t i = 0; i < m_logBuffer.size(); ++i)
		{
			ImGui::Text(m_logBuffer[i].c_str());
		}

		if (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		{
			ImGui::SetScrollHereY(1.0f);
		}
	}
	ImGui::EndChild();

	ImGui::End();
}
