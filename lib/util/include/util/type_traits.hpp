#ifndef __VTX_UTIL_TYPE_TRAITS__
#define __VTX_UTIL_TYPE_TRAITS__

#include "color/rgba.hpp"
#include "types.hpp"

namespace VTX
{

	template<class T>
	using is_vec2f = std::is_same<std::remove_cvref_t<T>, Vec2f>;
	template<class T>
	using is_vec2i = std::is_same<std::remove_cvref_t<T>, Vec2i>;
	template<class T>
	using is_vec3f = std::is_same<std::remove_cvref_t<T>, Vec3f>;
	template<class T>
	using is_vec3i = std::is_same<std::remove_cvref_t<T>, Vec3i>;
	template<class T>
	using is_color4 = std::is_same<std::remove_cvref_t<T>, Util::Color::Rgba>;

	template<class T>
	inline constexpr bool is_vec2f_v = is_vec2f<T>::value;
	template<class T>
	inline constexpr bool is_vec2i_v = is_vec2i<T>::value;
	template<class T>
	inline constexpr bool is_vec3f_v = is_vec3f<T>::value;
	template<class T>
	inline constexpr bool is_vec3i_v = is_vec3i<T>::value;
	template<class T>
	inline constexpr bool is_color4_v = is_color4<T>::value;

} // namespace VTX

#endif
