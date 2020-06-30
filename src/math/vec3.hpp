#ifndef __VTX__VEC3__
#define __VTX__VEC3__

#ifdef _MSC_VER
#pragma once
#endif

#include <glm/glm.hpp>

namespace VTX
{
	namespace Math
	{
		template<typename T, glm::qualifier Q>
		struct Vec3 : public glm::vec<3, T, Q>
		{
			Vec3() = default;
			Vec3( const T p_x, const T p_y, const T p_z ) : glm::vec<3, T, Q>( p_x, p_y, p_z ) {};
			Vec3( const T p_value ) : glm::vec<3, T, Q>( p_value ) {};
			~Vec3() = default;
		};
	} // namespace Math
} // namespace VTX

#endif
