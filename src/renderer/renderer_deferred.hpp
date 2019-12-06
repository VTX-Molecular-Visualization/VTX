#ifndef __VTX_RENDERER_DEFERRED__
#define __VTX_RENDERER_DEFERRED__

#ifdef _MSC_VER
#pragma once
#endif

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

			virtual void   init( Object3D::Scene * const, const View::BaseView3DMolecule::REPRESENTATION ) override;
			virtual void   clear() override;
			virtual double render( const Object3D::Scene * const ) override;
		};
	} // namespace Renderer
} // namespace VTX

#endif
