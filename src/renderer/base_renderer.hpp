#ifndef __VTX_BASE_RENDERER__
#define __VTX_BASE_RENDERER__

namespace VTX
{
	namespace Renderer
	{
		enum MODE
		{
			FORWARD = 0,
			DEFERRED
		};

		enum class SHADING
		{
			LAMBERT,
			BLINN_PHONG,
			TOON
		};

		class BaseRenderer
		{
		};
	} // namespace Renderer
} // namespace VTX

#endif
