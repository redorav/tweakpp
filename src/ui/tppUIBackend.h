#pragma once

struct ImGuiStyle;

namespace tpp
{
	enum class UIBackendResult
	{
		Quit,
		Ok,
	};

	struct UIInitializeParams
	{
		int windowPositionX;
		int windowPositionY;
		int windowWidth;
		int windowHeight;
	};

	class UIBackend
	{
	public:

		
		static bool Initialize(const UIInitializeParams& params);

		static bool Shutdown();

		static bool Render();

		static UIBackendResult PrepareNewFrame();

		static int GetWindowWidth();

		static int GetWindowHeight();

	private:

		static bool InitializePlatform(const UIInitializeParams& params);

		static void ApplyStyles(ImGuiStyle* dst = nullptr);

		static bool ShutdownPlatform();

		static bool RenderPlatform();

		static UIBackendResult PrepareNewFramePlatform();
	};
}