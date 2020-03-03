#ifndef __VTX_RENDERER_RAYTRACING__
#define __VTX_RENDERER_RAYTRACING__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_renderer.hpp"

namespace VTX
{
	namespace Renderer
	{
		class RT : public BaseRenderer
		{
		  public:
			RT()  = default;
			~RT() = default;

			virtual void init( const Object3D::Scene &, const uint, const uint ) override;
			virtual void clear( const Object3D::Scene & ) override;
			virtual void render( const Object3D::Scene & ) override;

		  private:
		};
	} // namespace Renderer
} // namespace VTX

#endif
