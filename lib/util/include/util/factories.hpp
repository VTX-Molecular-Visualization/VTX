#ifndef __VTX_UTIL_FACTORIES__
#define __VTX_UTIL_FACTORIES__

#include <memory>

namespace VTX::Util::Factories
{
	template<typename T>
	concept ConceptInitializable = requires( T & t ) {
		{ t.init() } -> std::same_as<void>;
	};

	/**
	 * @brief Factory function that constructs an object and calls its init method.
	 * Expections handled with unique_ptr guard.
	 * @return a raw pointer.
	 */
	template<ConceptInitializable T, class... Args>
	[[nodiscard]] T * newInit( Args &&... args )
	{
		auto guard = std::unique_ptr<T>( new T( std::forward<Args>( args )... ) );
		guard->init();
		return guard.release();
	}

	/**
	 * @brief Factory function that constructs an object and calls its init method.
	 * @return a unique pointer.
	 */
	template<ConceptInitializable T, typename... Args>
	[[nodiscard]] std::unique_ptr<T> makeInit( Args &&... args )
	{
		auto ptr = std::make_unique<T>( std::forward<Args>( args )... );
		ptr->init();
		return ptr;
	}
} // namespace VTX::Util::Factories
#endif
