#ifndef __VTX_UTIL_TYPE_TRAITS__
#define __VTX_UTIL_TYPE_TRAITS__

#include "color/rgba.hpp"
#include "types.hpp"

namespace VTX
{

	template<typename>
	struct is_vec2f : std::false_type
	{
	};
	template<>
	struct is_vec2f<Vec2f> : std::true_type
	{
	};

	template<typename>
	struct is_vec2i : std::false_type
	{
	};
	template<>
	struct is_vec2i<Vec2i> : std::true_type
	{
	};

	template<typename>
	struct is_vec3f : std::false_type
	{
	};
	template<>
	struct is_vec3f<Vec3f> : std::true_type
	{
	};

	template<typename>
	struct is_vec3i : std::false_type
	{
	};
	template<>
	struct is_vec3i<Vec3i> : std::true_type
	{
	};

	template<typename>
	struct is_color4 : std::false_type
	{
	};
	template<>
	struct is_color4<Util::Color::Rgba> : std::true_type
	{
	};

} // namespace VTX

#endif
