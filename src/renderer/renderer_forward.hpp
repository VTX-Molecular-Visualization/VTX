#ifndef __VTX_RENDERER_FORWARD__
#define __VTX_RENDERER_FORWARD__

#include "base_renderer.hpp"

namespace VTX
{
	namespace Renderer
	{
		class RendererForward : public BaseRenderer
		{
		  public:
			virtual double render( const Object3D::Scene & p_scene ) override { return 0.0; };
		};
	} // namespace Renderer
} // namespace VTX

#endif
