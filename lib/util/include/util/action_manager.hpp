#ifndef VTX_UTIL_ACTION_MANAGER_HPP
#define VTX_UTIL_ACTION_MANAGER_HPP

#include "util/chrono.hpp"
#include "util/fmt/duration.hpp"
#include "util/hashing.hpp"
#include "util/logger.hpp"
#include "util/string.hpp"
#include "util/thread/thread_manager.hpp"
#include <concepts>
#include <string_view>
#include <type_traits>
#include <utility>

namespace VTX::Util
{
	/**
	 * @brief Concept for action that can be threadable.
	 */
	template<typename T>
	concept ThreadableAction = std::constructible_from<T, Thread::ThreadData>;

	/**
	 * @brief Handle the execution of all actions.
	 */
	class ActionManager
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		ActionManager() = default;

		explicit ActionManager( Thread::ThreadManager & p_threadManager ) : _threadManager( &p_threadManager ) {}

		ActionManager( const ActionManager & )			   = delete;
		ActionManager & operator=( const ActionManager & ) = delete;
		ActionManager( ActionManager && )				   = delete;
		ActionManager & operator=( ActionManager && )	   = delete;
		~ActionManager()								   = default;

		/**
		 * @brief Execute an action of type A with the given arguments.
		 */
		template<typename A, typename... Args>
			requires( not ThreadableAction<A> )
		void execute( Args &&... p_args ) const noexcept
		{
			_guard(
				typeName<A>(),
				[ & ]()
				{
					A a;
					_run( a, std::forward<Args>( p_args )... );
				}
			);
		}

		/**
		 * @brief Execute the given action, priorizing multithreading except when there is no gui.
		 */
		template<typename A, typename... Args>
			requires ThreadableAction<A>
		void execute( Args &&... p_args ) const noexcept
		{
			// Run now.
			if ( _threadManager == nullptr )
			{
				_guard(
					typeName<A>(),
					[ & ]()
					{
						A a;
						_run( a, std::forward<Args>( p_args )... );
					}
				);
			}
			// Create a new thread.
			else
			{
				_guard(
					typeName<A>(),
					[ & ]()
					{
						_threadManager->createThread(
							[... args = std::forward<Args>( p_args ) ](
								Thread::StopToken p_token, Thread::BaseThread & p_thr
							) mutable
							{
								const bool succeeded = _guard(
									typeName<A>(),
									[ & ]()
									{
										A action( Thread::ThreadData { std::move( p_token ), p_thr } );
										action.execute( std::move( args )... );
									}
								);
								if ( not succeeded )
								{
									p_thr.setFailed();
								}
								return 0;
							}
						);
					}
				);
			}
		}

		/**
		 * @brief Execute the given action right away.
		 */
		template<typename A, typename... Args>
		void execute( A & p_action, Args &&... p_args ) const noexcept
		{
			_guard( typeName<A>(), [ & ]() { _run( p_action, std::forward<Args>( p_args )... ); } );
		}

		/**
		 * @brief Execute now without exception handling.
		 * Used by python to convert to his own exceptions.
		 */
		template<typename A, typename... Args>
		static void run( A & p_action, Args &&... p_args )
		{ _run( p_action, std::forward<Args>( p_args )... ); }

	  private:
		/**
		 * @brief Multi threading usage.
		 */
		Thread::ThreadManager * _threadManager = nullptr;

		/**
		 * @brief Execute now.
		 */
		template<typename A, typename... Args>
		static void _run( A & p_action, Args &&... p_args )
		{
			const auto actionName = typeName<A>();
			VTX_TRACE( "ActionSystem::execute( {} )", actionName );
			auto duration = CHRONO_CPU( [ & ]() { p_action.execute( std::forward<Args>( p_args )... ); } );
			VTX_TRACE( "ActionSystem::execute( {} ) - done ({})", actionName, String::FormattedDuration { duration } );
		}

		/**
		 * @brief Handle action exceptions.
		 */
		template<typename F>
		static bool _guard( const std::string_view p_actionName, F && p_function ) noexcept
		{
			try
			{
				std::forward<F>( p_function )();
				return true;
			}
			catch ( ... )
			{
				LOGGER::logException( p_actionName );
			}
			return false;
		}
	};
} // namespace VTX::Util

#endif
