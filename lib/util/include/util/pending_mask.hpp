#ifndef VTX_UTIL_PENDING_MASK_HPP
#define VTX_UTIL_PENDING_MASK_HPP

#include "util/enum.hpp"
#include "util/types.hpp"
#include <atomic>
#include <concepts>
#include <type_traits>

namespace VTX::Util
{
	/**
	 * @brief Callback.
	 */
	class WakeCallback
	{
	  public:
		/**
		 * @brief Function pointer.
		 */
		using Function = void ( * )( void * ) noexcept;

		/**
		 * @brief Set data.
		 */
		void set( const Function p_function, void * const p_context ) noexcept
		{ _data.store( Data { p_function, p_context }, std::memory_order_release ); }

		/**
		 * @brief Run.
		 */
		void operator()() const noexcept
		{
			const Data data = _data.load( std::memory_order_acquire );
			if ( data.function != nullptr )
			{
				data.function( data.context );
			}
		}

	  private:
		/**
		 * @brief Data structure.
		 */
		struct Data
		{
			/**
			 * @brief Function to call.
			 */
			Function function = nullptr;

			/**
			 * @brief Context to pass.
			 */
			void * context = nullptr;
		};

		/**
		 * @brief Thread-safe data.
		 */
		std::atomic<Data> _data = Data {};
	};

	/**
	 * @brief Wakeable concept.
	 */
	template<typename T>
	concept Wakeable = requires( T & p_target, const WakeCallback::Function p_function, void * const p_context ) {
		{ p_target.setWake( p_function, p_context ) } noexcept -> std::same_as<void>;
	};

	/**
	 * @brief Handle a dirty flag to specify data to refresh.
	 */
	template<Enum::Concept E>
		requires entt::enum_as_bitmask_v<E>
	class PendingMask
	{
	  public:
		using WakeFn = WakeCallback::Function;

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
				_wake();
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
		 * @brief
		 */
		void setWake( const WakeFn p_fn, void * const p_context ) noexcept { _wake.set( p_fn, p_context ); }

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
		WakeCallback _wake;
	};
} // namespace VTX::Util

#endif
