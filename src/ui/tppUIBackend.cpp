#include "tppUIBackend.h"

#include "tppUITextIcons.h"
#include "platform/tppPlatform.h"

#include "imgui.h"
#include "imgui_freetype.h"

// Copied from imgui_internal
static inline ImVec4 ImLerp(const ImVec4& a, const ImVec4& b, float t) { return ImVec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t); }
static inline ImVec4 operator*(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }
static inline ImVec4 operator/(const ImVec4& lhs, float rhs) { return ImVec4(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs); }

// Only used for lightening up or darkening colors
static const ImVec4 White            = ImVec4(255.0f, 255.0f, 255.0f, 255.0f) / 255.0f;
static const ImVec4 Black            = ImVec4(000.0f, 000.0f, 000.0f, 000.0f) / 255.0f;

static const ImVec4 BackgroundDark   = ImVec4(010.0f, 013.0f, 016.0f, 255.0f) / 255.0f;
static const ImVec4 MenuBarGrey      = ImVec4(040.0f, 043.0f, 046.0f, 255.0f) / 255.0f;
static const ImVec4 MenuBarDarkGrey  = ImVec4(024.0f, 027.0f, 030.0f, 255.0f) / 255.0f;

static const ImVec4 Blue             = ImVec4(016.0f, 170.0f, 255.0f, 255.0f) / 255.0f;
static const ImVec4 SelectionBlue    = ImVec4(000.0f, 067.0f, 115.0f, 255.0f) / 255.0f;

static const ImVec4 SliderGrab       = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
static const ImVec4 SliderGrabActive = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
static const ImVec4 Checkmark        = ImVec4(1.00f, 1.00f, 1.00f, 0.80f);
static const ImVec4 Text             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
static const ImVec4 TextDisabled     = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);

static const ImVec4 Scrollbar        = ImVec4(150.0f, 153.0f, 156.0f, 255.0f) / 255.0f;
static const ImVec4 ScrollbarHover   = ImVec4(180.0f, 183.0f, 186.0f, 255.0f) / 255.0f;
static const ImVec4 ScrollbarDrag    = ImVec4(220.0f, 223.0f, 226.0f, 255.0f) / 255.0f;

static const ImVec4 Button           = ImVec4(060.0f, 063.0f, 066.0f, 255.0f) / 255.0f;
static const ImVec4 ButtonHover      = ImVec4(074.0f, 077.0f, 080.0f, 255.0f) / 255.0f;
static const ImVec4 ButtonPress      = ImVec4(090.0f, 093.0f, 096.0f, 255.0f) / 255.0f;

void tpp::UIBackend::ApplyStyles(ImGuiStyle* dst)
{
	ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
	ImVec4* colors = style->Colors; 
	
	// colors[ImGuiCol_Text]                   = Text;
	// colors[ImGuiCol_TextDisabled]           = TextDisabled;
	// 
	// // General background of a window
	// colors[ImGuiCol_WindowBg]               = BackgroundDark;
	// 
	// // Background of a child window
	// colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	// 
	// // Background of a popup
	// colors[ImGuiCol_PopupBg]                = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
	// 
	// // 1-pixel border of a window
	// colors[ImGuiCol_Border]                 = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
	// 
	// // Shadow of the border
	// colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	// 
	// // Background of the widgets
	// colors[ImGuiCol_FrameBg]                = Blue;//ImVec4(1.43f, 0.43f, 0.43f, 0.39f);
	// colors[ImGuiCol_FrameBgHovered]         = ImLerp(Blue, White, 0.2f); // ImVec4(0.47f, 0.47f, 0.69f, 0.40f);
	// colors[ImGuiCol_FrameBgActive]          = ImLerp(Blue, White, 0.05f); // ImVec4(0.42f, 0.41f, 0.64f, 0.69f);
	// 
	// // Background of the title
	// colors[ImGuiCol_TitleBg]                = MenuBarGrey; // ImVec4(0.27f, 0.27f, 0.54f, 0.83f);
	// colors[ImGuiCol_TitleBgActive]          = MenuBarDarkGrey; // ImVec4(0.32f, 0.32f, 0.63f, 0.87f);
	// colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
	// 
	// // Background of the menu
	// colors[ImGuiCol_MenuBarBg]              = Blue; // Menu bar with File, About, etc
	// 
	// // Scrollbar Colors
	// colors[ImGuiCol_ScrollbarBg]            = MenuBarGrey;
	// colors[ImGuiCol_ScrollbarGrab]          = Scrollbar;
	// colors[ImGuiCol_ScrollbarGrabHovered]   = ScrollbarHover;
	// colors[ImGuiCol_ScrollbarGrabActive]    = ScrollbarDrag;
	// 
	// colors[ImGuiCol_CheckMark]              = Checkmark;
	// 
	// // Slider Colors (float, int, etc sliders)
	// // They are translucent white to integrate with the colors below
	// colors[ImGuiCol_SliderGrab]             = SliderGrab;
	// colors[ImGuiCol_SliderGrabActive]       = SliderGrabActive;
	// 
	// // Button Colors
	// colors[ImGuiCol_Button]                 = Button; // ImVec4(0.35f, 0.40f, 0.61f, 0.62f);
	// colors[ImGuiCol_ButtonHovered]          = ButtonHover; // ImVec4(0.40f, 0.48f, 0.71f, 0.79f);
	// colors[ImGuiCol_ButtonActive]           = ButtonPress; // ImVec4(0.46f, 0.54f, 0.80f, 1.00f);
	// 
	// colors[ImGuiCol_Header]                 = ImVec4(0.40f, 0.40f, 0.90f, 0.45f);
	// colors[ImGuiCol_HeaderHovered]          = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
	// colors[ImGuiCol_HeaderActive]           = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
	// 
	// colors[ImGuiCol_Separator]              = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
	// colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
	// colors[ImGuiCol_SeparatorActive]        = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
	// 
	// colors[ImGuiCol_ResizeGrip]             = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	// colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
	// colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.78f, 0.82f, 1.00f, 0.90f);
	// 
	// colors[ImGuiCol_Tab]                    = MenuBarDarkGrey; //ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.80f);
	// colors[ImGuiCol_TabHovered]             = ImLerp(MenuBarDarkGrey, White, 0.2f); // colors[ImGuiCol_HeaderHovered];
	// colors[ImGuiCol_TabActive]              = MenuBarGrey;// ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
	// colors[ImGuiCol_TabUnfocused]           = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
	// colors[ImGuiCol_TabUnfocusedActive]     = ImLerp(colors[ImGuiCol_TabActive],    colors[ImGuiCol_TitleBg], 0.40f);
	// 
	// colors[ImGuiCol_DockingPreview]         = colors[ImGuiCol_Header] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	// colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	// 
	// colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	// colors[ImGuiCol_PlotLinesHovered]       = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	// colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	// colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	// 
	// colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.27f, 0.27f, 0.38f, 1.00f);
	// colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.45f, 1.00f);   // Prefer using Alpha=1.0 here
	// colors[ImGuiCol_TableBorderLight]       = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);   // Prefer using Alpha=1.0 here
	// colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	// colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
	// 
	// colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
	// colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	// colors[ImGuiCol_NavHighlight]           = colors[ImGuiCol_HeaderHovered];
	// colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	// colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	// colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
	
	// Rectangular windows
	style->ChildRounding = 0.0f;
	
	// Rectangular tabs
	style->TabRounding = 0.0f;
}

static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
bool tpp::UIBackend::Initialize(const UIInitializeParams& params)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// ImFont* font = io.Fonts->AddFontDefault();
	// Add character ranges and merge into the previous font
	// The ranges array is not copied by the AddFont* functions and is used lazily
	// so ensure it is available at the time of building or calling GetTexDataAsRGBA32().

	float baseFontSize = 16.0f;

	std::string executablePath = tpp::Platform::GetExecutableDirectory();

	std::string SegoeUIFile = executablePath + "segoeui.ttf";
	std::string FontAwesomeFile = executablePath + "Font Awesome 6 Free-Solid-900.otf";
	std::string TwemojiFile = executablePath + "TwemojiMozilla.ttf";

	//-----------------
	// 1. Add base font
	//-----------------

	ImFontConfig defaultFontConfig;
	defaultFontConfig.SizePixels = baseFontSize;
	defaultFontConfig.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_LoadColor;
	io.Fonts->AddFontFromFileTTF(SegoeUIFile.c_str(), 0.0f, &defaultFontConfig, io.Fonts->GetGlyphRangesDefault());

	//---------------------------
	// 2. Add FontAwesome's icons
	//---------------------------

	ImVector<ImWchar> fontAwesomeRanges;
	{
		//static const ImWchar fontAwesomeIconsRanges[] = { 0xf000, 0xf3ff, 0 };
		ImFontGlyphRangesBuilder fontAwesomeBuilder;
		fontAwesomeBuilder.AddText(tpp::icons::FileCode);
		fontAwesomeBuilder.AddText(tpp::icons::MagnifyingGlass);
		fontAwesomeBuilder.AddText(tpp::icons::ElectricPlug);
		fontAwesomeBuilder.AddText(tpp::icons::RotatedStar);
		fontAwesomeBuilder.AddText(tpp::icons::NormalStar);
		fontAwesomeBuilder.AddText(tpp::icons::SettingsCog);
		fontAwesomeBuilder.AddText(tpp::icons::Info);

		fontAwesomeBuilder.BuildRanges(&fontAwesomeRanges);

		ImFontConfig fontAwesomeConfig;
		fontAwesomeConfig.SizePixels = baseFontSize;
		fontAwesomeConfig.MergeMode = true;
		fontAwesomeConfig.GlyphOffset = ImVec2(0.0f, 1.0f);
		fontAwesomeConfig.GlyphRanges = fontAwesomeRanges.Data;
		io.Fonts->AddFontFromFileTTF(FontAwesomeFile.c_str(), baseFontSize, &fontAwesomeConfig);
	}

	ImVector<ImWchar> fontAwesomeSmallRanges;
	{
		ImFontGlyphRangesBuilder fontAwesomeBuilder;
		fontAwesomeBuilder.AddText(tpp::icons::SquareWithMinus);
		fontAwesomeBuilder.AddText(tpp::icons::SquareWithPlus);

		fontAwesomeBuilder.BuildRanges(&fontAwesomeSmallRanges);

		float fontSize = baseFontSize * 0.8f;

		ImFontConfig fontAwesomeConfig;
		fontAwesomeConfig.SizePixels = fontSize;
		fontAwesomeConfig.MergeMode = true;
		fontAwesomeConfig.GlyphOffset = ImVec2(0.0f, 0.0f);
		fontAwesomeConfig.GlyphRanges = fontAwesomeSmallRanges.Data;
		io.Fonts->AddFontFromFileTTF(FontAwesomeFile.c_str(), fontSize, &fontAwesomeConfig);
	}

	//---------------------------------------------------
	// 3. Add Twemoji Font
	// https://github.com/mozilla/twemoji-colr
	// Twemoji is open source, whereas Segoe Emoji is not
	//---------------------------------------------------

	ImVector<ImWchar> twemojiRanges;
	{
		ImFontGlyphRangesBuilder twemojiBuilder;
		twemojiBuilder.AddText(tpp::icons::LargeRedCircle);
		twemojiBuilder.AddText(tpp::icons::LargeGreenCircle);
		twemojiBuilder.AddText(tpp::icons::LargeRedSquare);
		twemojiBuilder.BuildRanges(&twemojiRanges);

		float fontSize = baseFontSize * 0.6f;

		ImFontConfig twemojiConfig = {};
		twemojiConfig.SizePixels = fontSize;
		twemojiConfig.MergeMode = true;
		twemojiConfig.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_LoadColor;
		twemojiConfig.GlyphRanges = twemojiRanges.Data;
		twemojiConfig.GlyphOffset = ImVec2(0.0f, -1.0f);
		//fontAwesomeConfig.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint;
		io.Fonts->AddFontFromFileTTF(TwemojiFile.c_str(), fontSize, &twemojiConfig);
	}

	io.Fonts->Build();

	// Setup Dear ImGui style

	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	//ImGui::StyleColorsLight();
	ApplyStyles();

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
