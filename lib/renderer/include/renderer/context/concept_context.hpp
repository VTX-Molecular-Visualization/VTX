#ifndef __VTX_RENDERER_CONTEXT_CONCEPT__
#define __VTX_RENDERER_CONTEXT_CONCEPT__

#include <concepts>

namespace VTX::Renderer::Context
{

	template<typename C>
	concept Concept = requires( C p_context ) {
		{
			p_context.addTruc()
		} -> std::same_as<void>;
		{
			p_context.removeTruc()
		} -> std::same_as<void>;
	};

} // namespace VTX::Renderer::Context

#endif
