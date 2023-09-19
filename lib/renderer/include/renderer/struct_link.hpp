#ifndef __VTX_RENDERER_LINK__
#define __VTX_RENDERER_LINK__

#include "struct_pass.hpp"

namespace VTX::Renderer
{
	struct Link
	{
		Pass *	  src;
		Pass *	  dest;
		E_CHANNEL channelSrc  = E_CHANNEL::COLOR_0;
		E_CHANNEL channelDest = E_CHANNEL::COLOR_0;
	};
	using Links = std::vector<std::unique_ptr<Link>>;
} // namespace VTX::Renderer

#endif
