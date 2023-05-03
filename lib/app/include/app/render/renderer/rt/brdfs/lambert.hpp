#ifndef __VTX_APP_RENDER_RENDERER_RT_BRDF_LAMBERT__
#define __VTX_APP_RENDER_RENDERER_RT_BRDF_LAMBERT__

#include "../intersection.hpp"
#include <util/math.hpp>

namespace VTX::App::Render::Renderer::RT::BRDF
{
	namespace Lambert
	{
		inline Util::Color::Rgba fr( const Intersection &	   p_hit,
									 const Vec3f &			   p_wo,
									 const Vec3f &			   p_wi,
									 const Util::Color::Rgba & p_albedo )
		{
			return p_albedo * INV_PIf;
		}
	} // namespace Lambert

} // namespace VTX::App::Render::Renderer::RT::BRDF

#endif
