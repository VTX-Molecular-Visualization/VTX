#ifndef __VTX_APP_OLD_RENDER_RENDERER_RT_LIGHT_SAMPLE__
#define __VTX_APP_OLD_RENDER_RENDERER_RT_LIGHT_SAMPLE__

#include <util/color/rgba.hpp>
#include <util/types.hpp>

namespace VTX::App::Old::Render::Renderer::RT::Light
{
	struct LightSample
	{
		LightSample( const Vec3f &			   p_dir,
					 const float			   p_distance,
					 const Util::Color::Rgba & p_radiance,
					 const float			   p_pdf ) :
			_dir( p_dir ),
			_distance( p_distance ), _radiance( p_radiance ), _pdf( p_pdf )
		{
		}

		Vec3f			  _dir		= VEC3F_ZERO;				// normalized direction toward light source
		float			  _distance = 0.f;						// dir length
		Util::Color::Rgba _radiance = Util::Color::Rgba::BLACK; // radiance at given point divided by pdf
		float			  _pdf		= 1.f;
	};
} // namespace VTX::App::Old::Render::Renderer::RT::Light

#endif
