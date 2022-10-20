#include <stdio.h>

#include "tppTypes.h"
#include "server/tppServer.h"

//---------
// EXAMPLES
//---------

// SSR
tpp::Float SSRNumberOfRays("Rendering/Post Effects/SSR/Number of Rays", 8.0f, 1.0f, 64.0f, 1.0f);
tpp::UInt SSRThicknessMultiplier("Rendering/Post Effects/SSR/Thickness Multiplier", 1, 1, 8, 1);
tpp::Int SSRThicknessBias("Rendering/Post Effects/SSR/Thickness Bias", -1, -10, 10, 2);
tpp::Bool SSREnabled("Rendering/Post Effects/SSR/Enabled", false);
tpp::Vector2 SSRDirection2("Rendering/Post Effects/SSR/Direction 2", 1.0f, 0.7f);
tpp::Vector3 SSRDirection3("Rendering/Post Effects/SSR/Direction 3", 0.4f, 0.2f, 0.3f);
tpp::Vector4 SSRDirection4("Rendering/Post Effects/SSR/Direction 4", 0.34f, 0.5f, 0.6f, 0.1f);
tpp::Color3 SSRClearColor("Rendering/Post Effects/SSR/Clear Color", 1.0f, 0.5f, 0.3f);
tpp::Color4 SSRRayColor("Rendering/Post Effects/SSR/Ray Color", 0.7f, 0.4f, 0.2f, 0.2f);

tpp::Float SSRNumberOfRays1("Rendering/Post Effects/SSR/Number of Rays1", 8.0f, 1.0f, 64.0f, 1.0f);
tpp::Float SSRNumberOfRays2("Rendering/Post Effects/SSR/Number of Rays2", 8.0f, 1.0f, 64.0f, 1.0f);
tpp::Float SSRNumberOfRays3("Rendering/Post Effects/SSR/Number of Rays3", 8.0f, 1.0f, 64.0f, 1.0f);
tpp::Float SSRNumberOfRays4("Rendering/Post Effects/SSR/Number of Rays4", 8.0f, 1.0f, 64.0f, 1.0f);
tpp::Float SSRNumberOfRays5("Rendering/Post Effects/SSR/Number of Rays5", 8.0f, 1.0f, 64.0f, 1.0f);
tpp::Float SSRNumberOfRays6("Rendering/Post Effects/SSR/Number of Rays6", 8.0f, 1.0f, 64.0f, 1.0f);
tpp::Float SSRNumberOfRays7("Rendering/Post Effects/SSR/Number of Rays7", 8.0f, 1.0f, 64.0f, 1.0f);
tpp::Float SSRNumberOfRays8("Rendering/Post Effects/SSR/Number of Rays8", 8.0f, 1.0f, 64.0f, 1.0f);
tpp::Float SSRNumberOfRays9("Rendering/Post Effects/SSR/Number of Rays9", 8.0f, 1.0f, 64.0f, 1.0f);

void RecompileShadersCallback()
{
	printf("I recompiled shaders\n");
}

namespace SSRModes
{
	enum T
	{
		Reference,
		Preblur,
		Postblur,
		Stochastic
	};
};

std::vector<tpp::EnumEntry> RenderingModeEntries =
{
	{ SSRModes::Reference,  "Reference" },
	{ SSRModes::Preblur,    "Preblur" },
	{ SSRModes::Postblur,   "Postblur" },
	{ SSRModes::Stochastic, "Stochastic" }
};

tpp::Enum SSRRenderingMode("Rendering/Post Effects/SSR/Rendering Mode", SSRModes::Preblur, RenderingModeEntries);

namespace SSRRenderFlags
{
	enum T
	{
		DoIntersection   = 1,
		FireflyReduction = 2,
		UseLastFrame     = 3,
		UseFullresDepth  = 4
	};
};

std::vector<std::string> FlagOptionEntries =
{
	{ "Do Intersection" },
	{ "Firefly Reduction" },
	{ "Use Last Frame" },
	{ "Use Fullres Depth" },
};

tpp::Flags8 SSRFlagOptions8("Rendering/Post Effects/SSR/Rendering Flag Options 8", 0b0101, FlagOptionEntries);

tpp::Flags16 SSRFlagOptions16("Rendering/Post Effects/SSR/Rendering Flag Options 16", 0b0101, FlagOptionEntries);

tpp::Flags32 SSRFlagOptions32("Rendering/Post Effects/SSR/Rendering Flag Options 32", 0b0101, FlagOptionEntries);

tpp::Flags64 SSRFlagOptions64("Rendering/Post Effects/SSR/Rendering Flag Options 64", 0b0101, FlagOptionEntries);

tpp::String SSRCompilationFilter("Rendering/Post Effects/SSR/Compilation Filter", "");

tpp::Callback RecompileShaders("Rendering/Post Effects/SSR/Recompile Shaders", RecompileShadersCallback);

// Depth of Field
tpp::Float DepthOfFieldAperture("Rendering/Post Effects/Depth of Field/Aperture", 2.0f, 0.001f, 8.0f, 1.0f);
tpp::Float DepthOfFieldBokehSize("Rendering/Post Effects/Depth of Field/Bokeh Size", 2.5f, 1.0f, 32.0f, 1.0f);

// TAA
tpp::Float TAAJitterX("Rendering/Post Effects/TAA/TAA Jitter X", 0.5f, 0.0f, 128.0f, 1.0f);
tpp::Float TAAJitterY("Rendering/Post Effects/TAA/TAA Jitter Y", 0.5f, 0.0f, 128.0f, 1.0f);

tpp::Float PerformanceGraphScaleX("Rendering/Performance/Graph Scale X", 1.5f, 0.1f, 4.0f, 1.0f);
tpp::Float PerformanceGraphScaleY("Rendering/Performance/Graph Scale Y", 1.5f, 0.1f, 4.0f, 1.0f);

tpp::Float CoreGraphScaleX("Core/Performance/Graph Scale X", 1.3f, 0.1f, 4.0f, 1.0f);
tpp::Float CoreGraphScaleY("Core/Performance/Graph Scale Y", 1.3f, 0.1f, 4.0f, 1.0f);

tpp::Float AnimationTimeScale("Animation/Time Scale", 1.0f, 0.0f, 2.0f, 1.0f);
tpp::Float AnimationThreshold("Animation/Threshold", 1.0f, 1.0f, 3.0f, 1.0f);

tpp::Float PhysicsTargetFPS("Physics/Target FPS", 60.0f, 1.0f, 120.0f, 1.0f);
tpp::Float PhysicsFPSLimit("Physics/Performance/FPS Limit", 120.0f, 0.0f, 120.0f, 1.0f);

tpp::Float DebugDisplayDeferredNormals("Rendering/Debug Display/Deferred/Normals", 0.77f, 0.0f, 1.0f, 1.0f);
tpp::Float DebugDisplayForwardAlbedo("Rendering/Debug Display/Forward/Albedo", 1.0f, 0.0f, 1.0f, 1.0f);

int main(int argc, char **argv)
{
	int port = 27001;

	if (argc > 1)
	{
		port = atoi(argv[1]);
	}

	tpp::NetworkAddress address("127.0.0.1", port);
	tpp::Server::Initialize(address);

	bool shutdown = false;

	while (!shutdown)
	{
		tpp::Server::Update();
	}

	tpp::Server::Shutdown();

	printf("Server closed successfully\n");

	return 0;
}