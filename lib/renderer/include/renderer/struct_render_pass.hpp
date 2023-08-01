#ifndef __VTX_RENDERER_RENDER_PASS__
#define __VTX_RENDERER_RENDER_PASS__

#include "struct_ressource.hpp"
#include <functional>
#include <map>
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

	using Inputs			 = std::map<E_INPUT_CHANNEL, StructResource * const>;
	using Output			 = StructResource;
	using CallbackPassSetup	 = std::function<void>();
	using CallbackPassRender = std::function<void>();

	struct StructRenderPass
	{
		E_PASS_TYPE		   type;
		Inputs			   inputs;
		Output			   output;
		CallbackPassSetup  cbSetup;
		CallbackPassRender cbRender;
	};

	using ListPasses = std::vector<StructRenderPass>;

} // namespace VTX::Renderer

#endif
