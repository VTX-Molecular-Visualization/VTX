#ifndef __VTX_RENDERER_RENDER_PASS__
#define __VTX_RENDERER_RENDER_PASS__

#include "struct_ressource.hpp"
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace VTX::Renderer
{
	enum struct E_PASS_TYPE
	{
		GRAPHIC,
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
			std::string		name;
			Context::DescIO desc;
		};

		using Inputs = std::unordered_map<E_INPUT_CHANNEL, Input>;

		struct Output
		{
			Resource *		resource;
			Context::DescIO desc;
		};

		// Type.
		// E_PASS_TYPE type;
		// In/Out.
		Inputs inputs;
		Output output;

		// Program.
		Context::DescProgram program;

		// Callbacks.
		// PassCallbackPassSetup  cbSetup;
		// PassCallbackPassRender cbRender;

		using CallbackSetup	 = std::function<void>();
		using CallbackRender = std::function<void>();
	};

	using Passes = std::unordered_map<std::string, Pass>;
} // namespace VTX::Renderer

#endif
