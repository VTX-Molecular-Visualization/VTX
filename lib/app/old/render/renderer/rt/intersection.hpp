#ifndef __VTX_APP_OLD_RENDER_RENDERER_RT_INTERSECTION__
#define __VTX_APP_OLD_RENDER_RENDERER_RT_INTERSECTION__

#include <util/constants.hpp>
#include <util/types.hpp>

namespace VTX::App::Old::Render::Renderer::RT
{
	namespace Primitive
	{
		class BasePrimitive;
	}

	struct Intersection
	{
		Intersection() = default;
		Intersection( const Vec3f &							 p_point,
					  const Vec3f &							 p_normal,
					  const float							 p_distance,
					  const Primitive::BasePrimitive * const p_prim ) :
			_point( p_point ),
			_normal( p_normal ), _distance( p_distance ), _primitive( p_prim )
		{
		}
		Intersection( const Intersection & p_intersection )				= default; // copy constructor
		Intersection( Intersection && p_intersection )					= default; // move constructor
		Intersection & operator=( const Intersection & p_intersection ) = default; // = copy
		Intersection & operator=( Intersection && p_intersection )		= default; // = move

		Vec3f							 _point		= VEC3F_ZERO;
		Vec3f							 _normal	= VEC3F_ZERO;
		float							 _distance	= -1.f;
		const Primitive::BasePrimitive * _primitive = nullptr;
	};
	static const Intersection NO_INTERSECTION = Intersection();
} // namespace VTX::App::Old::Render::Renderer::RT

#endif
