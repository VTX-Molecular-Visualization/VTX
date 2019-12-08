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

			virtual void init( Object3D::Scene * const ) override;
			virtual void clear( Object3D::Scene * const ) override;
			virtual void render( const Object3D::Scene * const, const uint ) override;
		};
	} // namespace Renderer
} // namespace VTX

#endif
