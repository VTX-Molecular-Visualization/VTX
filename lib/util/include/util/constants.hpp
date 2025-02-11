#ifndef __VTX_UTIL_CONSTANTS__
#define __VTX_UTIL_CONSTANTS__

#include "util/color/rgba.hpp"
#include "util/types.hpp"
#include <limits>

#ifdef __APPLE__
#undef SIZE_T_MIN
#undef SIZE_T_MAX
#endif

namespace VTX
{
	constexpr float FLOAT_LOWEST = std::numeric_limits<float>::lowest();
	constexpr float FLOAT_MIN	 = std::numeric_limits<float>::min();
	constexpr float FLOAT_MAX	 = std::numeric_limits<float>::max();

	constexpr uchar	 UNSIGNED_CHAR_MAX	= std::numeric_limits<uchar>::max();
	constexpr ushort UNSIGNED_SHORT_MAX = std::numeric_limits<ushort>::max();
	constexpr uint	 UNSIGNED_INT_MAX	= std::numeric_limits<uint>::max();

	constexpr size_t SIZE_T_MIN = std::numeric_limits<size_t>::min();
	constexpr size_t SIZE_T_MAX = std::numeric_limits<size_t>::max();

	constexpr float EPSILON = 1e-4f;

	constexpr Vec2i VEC2I_ZERO	 = Vec2i( 0, 0 );
	constexpr Vec3i VEC3I_ZERO	 = Vec3i( 0, 0, 0 );
	constexpr Vec3f VEC3F_ZERO	 = Vec3f( 0.f, 0.f, 0.f );
	constexpr Vec3f VEC3F_X		 = Vec3f( 1.f, 0.f, 0.f );
	constexpr Vec3f VEC3F_Y		 = Vec3f( 0.f, 1.f, 0.f );
	constexpr Vec3f VEC3F_Z		 = Vec3f( 0.f, 0.f, 1.f );
	constexpr Vec3f VEC3F_XYZ	 = Vec3f( 1.f, 1.f, 1.f );
	constexpr Vec3f VEC3F_LOWEST = Vec3f( FLOAT_LOWEST );
	constexpr Vec3f VEC3F_MAX	 = Vec3f( FLOAT_MAX );

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

	const Util::Color::Rgba COLOR_BLACK	  = Util::Color::Rgba( 0.f, 0.f, 0.f, 1.f );
	const Util::Color::Rgba COLOR_WHITE	  = Util::Color::Rgba( 1.f, 1.f, 1.f, 1.f );
	const Util::Color::Rgba COLOR_GREY	  = Util::Color::Rgba( 0.5f, 0.5f, 0.5f, 1.f );
	const Util::Color::Rgba COLOR_RED	  = Util::Color::Rgba( 1.f, 0.f, 0.f, 1.f );
	const Util::Color::Rgba COLOR_GREEN	  = Util::Color::Rgba( 0.f, 1.f, 0.f, 1.f );
	const Util::Color::Rgba COLOR_BLUE	  = Util::Color::Rgba( 0.f, 0.f, 1.f, 1.f );
	const Util::Color::Rgba COLOR_YELLOW  = Util::Color::Rgba( 1.f, 1.f, 0.f, 1.f );
	const Util::Color::Rgba COLOR_MAGENTA = Util::Color::Rgba( 1.f, 0.f, 1.f, 1.f );
	const Util::Color::Rgba COLOR_CYAN	  = Util::Color::Rgba( 0.f, 1.f, 1.f, 1.f );

	// constexpr uint	 INVALID_MODEL_ID = UNSIGNED_INT_MAX;

	constexpr size_t	   INVALID_INDEX	  = SIZE_T_MAX;
	constexpr atom_index_t INVALID_ATOM_INDEX = UNSIGNED_INT_MAX;

} // namespace VTX

#endif
