#ifndef VTX_UTIL_ATOMIC_CALLBACK_HPP
#define VTX_UTIL_ATOMIC_CALLBACK_HPP

#include <atomic>
#include <concepts>

namespace VTX::Util
{
	/**
	 * @brief Function pointer and context pair that can be set and invoked from any thread.
	 */
	class AtomicCallback
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
	 * @brief Type that exposes an optional callback invoked when work becomes pending.
	 */
	template<typename T>
	concept PendingNotifier
		= requires( T & p_target, const AtomicCallback::Function p_function, void * const p_context ) {
			  { p_target.setOnPending( p_function, p_context ) } noexcept -> std::same_as<void>;
		  };
} // namespace VTX::Util

#endif
