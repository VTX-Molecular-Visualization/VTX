#ifndef __VTX_UTIL_CONSTANTS__
#define __VTX_UTIL_CONSTANTS__

#include "util/color/rgba.hpp"
#include "util/types.hpp"
#include <limits>

namespace VTX
{
	template<typename T>
	constexpr T TypeMin = std::numeric_limits<T>::min();
	template<typename T>
	constexpr T TypeMax = std::numeric_limits<T>::max();
	template<typename T>
	constexpr T TypeLowest = std::numeric_limits<T>::lowest();
	template<typename T>
	constexpr size_t TypeCount = size_t { 1 } << std::numeric_limits<T>::digits;

	constexpr float EPSILON	 = 1e-4f;
	constexpr float _MS_TO_S = 1e-3f;

	constexpr Vec2i VEC2I_ZERO	 = Vec2i( 0, 0 );
	constexpr Vec3i VEC3I_ZERO	 = Vec3i( 0, 0, 0 );
	constexpr Vec3f VEC3F_ZERO	 = Vec3f( 0.f, 0.f, 0.f );
	constexpr Vec3f VEC3F_X		 = Vec3f( 1.f, 0.f, 0.f );
	constexpr Vec3f VEC3F_Y		 = Vec3f( 0.f, 1.f, 0.f );
	constexpr Vec3f VEC3F_Z		 = Vec3f( 0.f, 0.f, 1.f );
	constexpr Vec3f VEC3F_XYZ	 = Vec3f( 1.f, 1.f, 1.f );
	constexpr Vec3f VEC3F_LOWEST = Vec3f( TypeLowest<float> );
	constexpr Vec3f VEC3F_MAX	 = Vec3f( TypeMax<float> );

	constexpr Vec3f RIGHT_AXIS = VEC3F_X;
	constexpr Vec3f UP_AXIS	   = VEC3F_Y;
	constexpr Vec3f FRONT_AXIS = -VEC3F_Z;

	constexpr Vec4f VEC4F_ZERO = Vec4f( 0.f, 0.f, 0.f, 0.f );
	constexpr Vec4f VEC4F_XYZW = Vec4f( 1.f, 1.f, 1.f, 1.f );

	constexpr Mat3f MAT3F_ID = Mat3f( 1.f );
	constexpr Mat4f MAT4F_ID = Mat4f( 1.f );

	constexpr Quatf QUATF_ID = Quatf( 1.f, { 0.f, 0.f, 0.f } );

	constexpr float PIf		 = glm::pi<float>();
	constexpr float PI_2f	 = glm::half_pi<float>();
	constexpr float PI_4f	 = glm::quarter_pi<float>();
	constexpr float PI_32f	 = glm::three_over_two_pi<float>();
	constexpr float TWO_PIf	 = glm::two_pi<float>();
	constexpr float INV_PIf	 = glm::one_over_pi<float>();
	constexpr float INV_2PIf = glm::one_over_two_pi<float>();

	constexpr Util::Color::Rgba COLOR_BLACK	  = Util::Color::Rgba( 0.f, 0.f, 0.f, 1.f );
	constexpr Util::Color::Rgba COLOR_WHITE	  = Util::Color::Rgba( 1.f, 1.f, 1.f, 1.f );
	constexpr Util::Color::Rgba COLOR_GREY	  = Util::Color::Rgba( 0.5f, 0.5f, 0.5f, 1.f );
	constexpr Util::Color::Rgba COLOR_RED	  = Util::Color::Rgba( 1.f, 0.f, 0.f, 1.f );
	constexpr Util::Color::Rgba COLOR_GREEN	  = Util::Color::Rgba( 0.f, 1.f, 0.f, 1.f );
	constexpr Util::Color::Rgba COLOR_BLUE	  = Util::Color::Rgba( 0.f, 0.f, 1.f, 1.f );
	constexpr Util::Color::Rgba COLOR_YELLOW  = Util::Color::Rgba( 1.f, 1.f, 0.f, 1.f );
	constexpr Util::Color::Rgba COLOR_MAGENTA = Util::Color::Rgba( 1.f, 0.f, 1.f, 1.f );
	constexpr Util::Color::Rgba COLOR_CYAN	  = Util::Color::Rgba( 0.f, 1.f, 1.f, 1.f );

	constexpr Index INVALID_INDEX = TypeMax<Index>;
	constexpr int	INVALID_UID	  = 0;
} // namespace VTX

#endif
