#ifndef __VTX_RENDERER__
#define __VTX_RENDERER__

#include "base_renderer.hpp"

namespace VTX
{
	namespace Renderer
	{
		class Renderer : BaseRenderer
		{
			enum Shading
			{
				LAMBERT = 0,
				BLINN_PHONG,
				TOON
			};
		};
	} // namespace Renderer
} // namespace VTX

#endif
