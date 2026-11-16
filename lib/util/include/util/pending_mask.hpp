#ifndef VTX_UTIL_PENDING_MASK_HPP
#define VTX_UTIL_PENDING_MASK_HPP

#include "util/atomic_callback.hpp"
#include "util/enum.hpp"
#include "util/types.hpp"
#include <atomic>
#include <type_traits>

namespace VTX::Util
{
	/**
	 * @brief Handle a dirty flag to specify data to refresh.
	 */
	template<Enum::Concept E>
		requires entt::enum_as_bitmask_v<E>
	class PendingMask
	{
	  public:
		/**
		 * @brief Mark flag as dirty.
		 */
		void mark( const E p_flags ) noexcept
		{
			if ( not p_flags )
			{
				return;
			}

			const Underlying previous = _flags.fetch_or( toUnderlying( p_flags ), std::memory_order_acq_rel );
			if ( previous == Underlying {} )
			{
				_onPending();
			}
		}

		/**
		 * @brief Reset flags.
		 */
		E consume() noexcept { return static_cast<E>( _flags.exchange( Underlying {}, std::memory_order_acq_rel ) ); }

		/**
		 * @brief Check if any flag.
		 */
		bool any() const noexcept { return _flags.load( std::memory_order_acquire ) != Underlying {}; }

		/**
		 * @brief Set the callback invoked when the mask goes from empty to non-empty.
		 */
		void setOnPending( const AtomicCallback::Function p_function, void * const p_context ) noexcept
		{ _onPending.set( p_function, p_context ); }

	  private:
		/**
		 * @brief Underlying type.
		 */
		using Underlying = std::underlying_type_t<E>;

		/**
		 * @brief Current flags.
		 */
		std::atomic<Underlying> _flags = Underlying {};

		/**
		 * @brief Callback.
		 */
		AtomicCallback _onPending;
	};
} // namespace VTX::Util

#endif
