#ifndef __VTX_RENDERER_DEFERRED__
#define __VTX_RENDERER_DEFERRED__

#include "base_renderer.hpp"

namespace VTX
{
	namespace Renderer
	{
		class RendererDeferred : public BaseRenderer
		{
		  public:
			virtual double render( const Object3D::Scene & p_scene ) override { return 0.0; };
		};
	} // namespace Renderer
} // namespace VTX

#endif
