#ifndef __VTX_UTIL_CONCEPTS__
#define __VTX_UTIL_CONCEPTS__

#include <concepts>
#include <type_traits>

namespace VTX
{

	/**
	 * @brief Check if two types are the same, ignoring const, volatile and reference qualifiers.
	 */
	template<typename LEFT, typename RIGHT>
	concept SameUnalteredType = std::same_as<std::remove_cvref_t<LEFT>, std::remove_cvref_t<RIGHT>>;

	/**
	 * @brief Check if smart ptr (->get()).
	 */
	template<typename T>
	concept IsSmartPtr = requires( T t ) {
		typename T::element_type;
		{ t.get() } -> std::same_as<typename T::element_type *>;
	};

} // namespace VTX

#endif
