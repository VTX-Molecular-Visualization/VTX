#ifndef __VTX_UTIL_HASHING__
#define __VTX_UTIL_HASHING__

#include "type_traits.hpp"
#include "types.hpp"
#include <string>
#include <string_view>
#include <typeinfo>
#include <utility>

namespace VTX
{
	/**
	 * @brief Hashing.
	 */
	using Hash = size_t;
} // namespace VTX

namespace VTX::Util
{
	/**
	 * @brief Raw type.
	 */
	template<typename T>
	using DefaultHashProcess = std::remove_pointer_t<std::decay_t<T>>;

	/**
	 * @brief Hash a value using std::hash.
	 * noexcept if std::hash<T>()( p_value ) is noexcept for T.
	 */
	template<typename T>
	Hash hash( const T & p_value ) noexcept( noexcept( std::hash<T>()( p_value ) ) )
	{ return std::hash<T>()( p_value ); }

	/**
	 * @brief Specialization for const char *.
	 */
	inline Hash hash( const char * const p_value ) noexcept
	{ return std::hash<std::string_view>()( std::string_view( p_value ) ); }

	/**
	 * @brief Hash from hash code.
	 */
	template<typename T>
	Hash hash() noexcept
	{ return typeid( DefaultHashProcess<T> ).hash_code(); }

	/**
	 * @brief Hash from name.
	 */
	template<typename T>
	std::string_view typeName() noexcept
	{ return typeid( DefaultHashProcess<T> ).name(); }

} // namespace VTX::Util
#endif