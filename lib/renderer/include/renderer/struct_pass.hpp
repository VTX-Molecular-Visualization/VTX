#ifndef __VTX_RENDERER_RENDER_PASS__
#define __VTX_RENDERER_RENDER_PASS__

#include "struct_ressource.hpp"
#include <functional>
#include <map>
#include <memory>
#include <vector>

namespace VTX::Renderer
{
	enum struct E_PASS_TYPE
	{
		GRAPHIC,
		// GEOMETRIC?
		// COMPUTE,
		// ...?
	};

	enum struct E_INPUT_CHANNEL
	{
		COLOR_0,
		COLOR_1,
		COLOR_2,
		DEPTH,
	};

	struct Pass
	{
		struct Input
		{
			// TODO: descriptor.
			std::string name;
		};
		using Inputs = std::map<const E_INPUT_CHANNEL, Input>;

		struct Output
		{
			// GPU data.
			std::unique_ptr<Resource> resource;
		};

		// Type.
		// E_PASS_TYPE type;
		// In/Out.
		Inputs inputs;
		Output output;
		// Callbacks.
		// PassCallbackPassSetup  cbSetup;
		// PassCallbackPassRender cbRender;

		using CallbackSetup	 = std::function<void>();
		using CallbackRender = std::function<void>();
	};

	using Passes = std::map<const std::string, Pass>;
} // namespace VTX::Renderer

#endif