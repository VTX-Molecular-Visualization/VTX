#ifndef __VTX_APP_OLD_RENDER_RENDERER_RT_BRDF_PHONG__
#define __VTX_APP_OLD_RENDER_RENDERER_RT_BRDF_PHONG__

#include "../intersection.hpp"
#include <util/math.hpp>

namespace VTX::App::Old::Render::Renderer::RT::BRDF
{
	namespace Phong
	{
		inline Vec3f fr( const Intersection & p_hit,
						 const Vec3f &		  p_wo,
						 const Vec3f &		  p_wi,
						 const Vec3f &		  p_albedo,
						 const float &		  p_shininess )
		{
			const Vec3f reflect = Util::Math::normalize( Util::Math::reflect( -p_wi, p_hit._normal ) );
			return p_albedo * Util::Math::pow( Util::Math::max( 0.f, Util::Math::dot( reflect, p_wo ) ), p_shininess );
		}
	} // namespace Phong

} // namespace VTX::App::Old::Render::Renderer::RT::BRDF

#endif
