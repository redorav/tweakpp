#pragma once

// Collection of custom imgui widgets to be able to add icons, etc

typedef int ImGuiTreeNodeFlags;
typedef unsigned int ImGuiID;
typedef char* va_list;

namespace tpp
{
	namespace imgui
	{
		bool TreeNodeBehavior(ImGuiID id, const char* icon, ImGuiTreeNodeFlags flags, const char* label, const char* label_end = nullptr);

		bool TreeNodeEx(const char* label, const char* icon, ImGuiTreeNodeFlags flags = 0);

		bool TreeNodeEx(const char* str_id, const char* icon, ImGuiTreeNodeFlags flags, const char* fmt, ...);

		bool TreeNodeEx(const void* ptr_id, const char* icon, ImGuiTreeNodeFlags flags, const char* fmt, ...);

		bool TreeNodeExV(const char* str_id, const char* icon, ImGuiTreeNodeFlags flags, const char* fmt, va_list args);

		bool TreeNodeExV(const void* ptr_id, const char* icon, ImGuiTreeNodeFlags flags, const char* fmt, va_list args);
	}
}