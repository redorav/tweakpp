#pragma once

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

	private:

		static bool InitializePlatform(const UIInitializeParams& params);

		static bool ShutdownPlatform();

		static bool RenderPlatform();

		static UIBackendResult PrepareNewFramePlatform();
	};
}