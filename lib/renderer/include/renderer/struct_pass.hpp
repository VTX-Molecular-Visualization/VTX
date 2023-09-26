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
		// GEOMETRIC,
		GRAPHIC,
		// COMPUTE,
		// ...?
	};

	enum struct E_CHANNEL
	{
		COLOR_0,
		COLOR_1,
		COLOR_2,
		DEPTH,
	};

	struct Pass
	{
		std::string name;
		struct Input
		{
			std::string		name;
			Context::DescIO desc;
		};

		struct Output : public Input
		{
		};

		using Inputs   = std::unordered_map<E_CHANNEL, Input>;
		using Outputs  = std::unordered_map<E_CHANNEL, Output>;
		using Programs = std::vector<Context::DescProgram>;

		Inputs	 inputs;
		Outputs	 outputs;
		Programs programs;

		// Callbacks.
		// PassCallbackPassSetup  cbSetup;
		// PassCallbackPassRender cbRender;

		using CallbackSetup	 = std::function<void>();
		using CallbackRender = std::function<void>();
	};

	using Passes = std::vector<std::unique_ptr<Pass>>;
} // namespace VTX::Renderer

#endif
