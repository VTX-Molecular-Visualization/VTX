#ifndef __VTX_RENDERER_CONTEXT_CONCEPT__
#define __VTX_RENDERER_CONTEXT_CONCEPT__

#include <concepts>

namespace VTX::Renderer::Context
{
	enum struct E_FORMAT
	{
		RGBA16F,
		RGBA32UI,
	};

	enum struct E_ACCESS
	{
		READ,
		WRITE,
		READ_WRITE,
	};

	// Descriptors.
	struct Texture
	{
		E_FORMAT format;
	};

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
