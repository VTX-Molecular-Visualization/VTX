#ifndef __VTX_UTIL_PLAYERS__
#define __VTX_UTIL_PLAYERS__

#include "util/types.hpp"
#include <concepts>

namespace VTX::Util
{
	class Player
	{
	  public:
		/**
		 * @brief Set a new current step. Jump to last step if input > num step.
		 * @param p_step New current step
		 */
		inline void jumpTo( const uint & p_step ) noexcept { _ptr->jumpTo( p_step ); }
		/**
		 * @brief Returns the next step. Does not change internal state.
		 * @param p_out
		 */
		inline void next( uint & p_out ) const noexcept { _ptr->next( p_out ); }
		/**
		 * @brief Move to the next step.
		 */
		inline void increment() noexcept { _ptr->increment(); }

	  private:
		struct _interface
		{
			virtual ~_interface()								= default;
			virtual void jumpTo( const uint & p_step ) noexcept = 0;
			virtual void next( uint & ) const noexcept			= 0;
			virtual void increment() noexcept					= 0;
		};
		struct _dummy
		{
		};
		template<typename T>
		class _wrapper final : public _interface
		{
			T _obj;

		  public:
			_wrapper( T && p_ ) : _obj( std::forward<T>( p_ ) ) {}
			virtual void jumpTo( const uint & p_step ) noexcept override
			{
				if constexpr ( not std::same_as<T, _dummy> )
				{
					_obj.jumpTo( p_step );
				}
			}
			virtual void next( uint & p_out ) const noexcept override
			{
				if constexpr ( not std::same_as<T, _dummy> )
				{
					_obj.next( p_out );
				}
			}
			virtual void increment() noexcept override
			{
				if constexpr ( not std::same_as<T, _dummy> )
				{
					_obj.increment();
				}
			}
		};
		std::unique_ptr<_interface> _ptr { std::make_unique<_wrapper<_dummy>>( _dummy() ) };

	  public:
		template<typename PlayerT>
		Player( PlayerT && p_ ) : _ptr( new _wrapper<PlayerT>( std::forward<PlayerT>( p_ ) ) )
		{
		}
		Player() = default;
	};

	namespace Players
	{
		class Forward
		{
		  public:
			Forward( uint p_stepNum, uint p_startingStep = 0 );
			void jumpTo( const uint & p_step ) noexcept;
			void next( uint & p_out ) const noexcept;
			void increment() noexcept;

		  private:
			uint _lastIndex	  = 0;
			uint _currentStep = 0;
		};
		class ForwardLoop
		{
		};
		class Backward
		{
		};
		class BackwardLoop
		{
		};
		class PingPong
		{
		  public:
			PingPong( uint p_stepNum, uint p_startingStep = 0 );
			void jumpTo( const uint & p_step ) noexcept;
			void next( uint & p_out ) const noexcept;
			void increment() noexcept;

		  private:
			uint _lastIndex	  = 0;
			uint _currentStep = 0;
			bool _forward	  = true;
		};
	} // namespace Players

} // namespace VTX::Util

#endif
