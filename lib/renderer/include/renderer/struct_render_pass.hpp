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

	struct StructRenderPassOutput
	{
		// GPU data.
		std::unique_ptr<Resource> resource;
	};

	using PassOutput = StructRenderPassOutput;

	struct StructRenderPassInput
	{
		// TODO: descriptor.
		std::string name;

		// Pointed resource.
		PassOutput * source = nullptr;
	};

	using PassInput	 = StructRenderPassInput;
	using PassInputs = std::map<const E_INPUT_CHANNEL, StructRenderPassInput>;

	using PassCallbackPassSetup	 = std::function<void>();
	using PassCallbackPassRender = std::function<void>();

	struct StructRenderPass
	{
		// Type.
		// E_PASS_TYPE type;
		// In/Out.
		PassInputs inputs;
		PassOutput output;
		// Callbacks.
		// PassCallbackPassSetup  cbSetup;
		// PassCallbackPassRender cbRender;
	};

	using Pass	 = StructRenderPass;
	using Passes = std::map<const std::string, Pass>;
} // namespace VTX::Renderer

#endif
