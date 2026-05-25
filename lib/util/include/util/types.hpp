#ifndef __VTX_UTIL_TYPES__
#define __VTX_UTIL_TYPES__

#include <cstdint>
#include <glm/detail/qualifier.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace VTX
{
	/**
	 * @brief Basic types.
	 */
	using uchar	 = uint8_t;
	using ushort = uint16_t;
	using uint	 = uint32_t;

	/**
	 * @brief GLM.
	 */
	using Vec2f = glm::vec2;
	using Vec2d = glm::dvec2;
	using Vec2i = glm::ivec2;
	using Vec3f = glm::vec3;
	using Vec3d = glm::dvec3;
	using Vec3i = glm::ivec3;
	using Vec3b = glm::bvec3;
	using Vec4f = glm::vec4;
	using Vec4d = glm::dvec4;
	using Vec4i = glm::ivec4;
	using Mat3f = glm::mat3;
	using Mat4f = glm::mat4;
	using Quatf = glm::quat;
	using Quatd = glm::dquat;

	/**
	 * @brief Index type used for local indexing (e.g. atoms in residue, etc...).
	 */
	using Index = uint32_t;

	/**
	 * @brief Convert enum to its underlying type.
	 */
	template<typename E>
		requires std::is_enum_v<E>
	constexpr std::underlying_type_t<E> toUnderlying( const E p_e ) noexcept
	{
		return static_cast<std::underlying_type_t<E>>( p_e );
	}

} // namespace VTX

#endif
