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
			RendererDeferred()	= default;
			~RendererDeferred() = default;

			virtual void   start( const Object3D::Scene *, const View::BaseView3DMolecule::REPRESENTATION ) override;
			virtual void   stop() override;
			virtual double render( const Object3D::Scene * ) override;
		};
	} // namespace Renderer
} // namespace VTX

#endif
