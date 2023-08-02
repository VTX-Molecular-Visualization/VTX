#ifndef __VTX_RENDERER_LINK__
#define __VTX_RENDERER_LINK__

#include "struct_pass.hpp"

namespace VTX::Renderer
{
	struct Link
	{
		Pass *			source;
		Pass *			destination;
		E_INPUT_CHANNEL channel;
	};

	using Links = std::vector<Link>;
} // namespace VTX::Renderer

#endif
