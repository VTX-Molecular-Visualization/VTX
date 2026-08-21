#ifndef __VTX_UTIL_THREAD_MANAGER__
#define __VTX_UTIL_THREAD_MANAGER__

#include "util/thread/base_thread.hpp"
#include <concepts>
#include <functional>
#include <future>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>
#include <unordered_set>
#include <utility>

namespace VTX::Util::Thread
{
	/**
	 * @brief Thread manager.
	 * To spawn a thread call the createThread method passing a function that will be called in the context of the
	 * thread.
	 */
	class ThreadManager
	{
		friend class BaseThread;

	  public:
		/**
		 * @brief Constructors.
		 */
		ThreadManager();
		ThreadManager( const ThreadManager & )			   = delete;
		ThreadManager & operator=( const ThreadManager & ) = delete;
		~ThreadManager();

		/**
		 * @brief Callback mutators.
		 */
		void setDefaultProgressCallback( BaseThread::ProgressCallback p_callback );
		void setDefaultTerminatedCallback( BaseThread::TerminatedCallback p_callback );

		/*
		 * @brief Spawn a thread and executes the content of p_asyncOp in the new thread context.
		 * Once the p_asyncOp has returned, p_callback is executed.
		 */
		BaseThread & createThread(
			const BaseThread::AsyncOp &		p_asyncOp,
			const BaseThread::EndCallback & p_callback = {}
		);

		/**
		 * @brief Creates a thread of type T.
		 */
		template<typename T, typename... Args>
			requires std::derived_from<T, BaseThread>
		std::shared_ptr<T> createThread( Args &&... p_args )
		{
			std::shared_ptr<T> thread = std::make_shared<T>( *this, std::forward<Args>( p_args )... );
			_threads.emplace_back( thread );
			try
			{
				thread->start();
			}
			catch ( ... )
			{
				_threads.pop_back();
				throw;
			}
			return thread;
		}

		/**
		 * @brief Synchronizes the execution of a function in the context of the thread that owns the ThreadManager.
		 * Called from another thread, this function will block until the function has been executed.
		 */
		template<typename Func>
		auto synchronize( Func && p_function ) -> std::invoke_result_t<std::decay_t<Func> &>
		{
			using Function = std::decay_t<Func>;
			using Result   = std::invoke_result_t<Function &>;

			Function			 function( std::forward<Func>( p_function ) );
			const BaseThread::ID caller = std::this_thread::get_id();
			if ( caller == _ownerThreadId )
			{
				return std::invoke( function );
			}

			std::future<Result> future;
			{
				std::packaged_task<Result()> resultTask( std::move( function ) );
				future = resultTask.get_future();
				_SynchronizedCall call(
					caller, std::packaged_task<void()>( [ task = std::move( resultTask ) ]() mutable { task(); } )
				);
				const std::scoped_lock lock( _synchronizedCallsMutex );
				if ( not _shuttingDown && not _cancelledCallers.contains( caller ) )
				{
					_synchronizedCalls.emplace_back( std::move( call ) );
				}
			}

			return future.get();
		}

		/**
		 * @brief Dispatches a function to be executed in the context of the thread that owns the ThreadManager.
		 * Called from another thread, this function will return immediately.
		 */
		template<typename Func>
			requires std::invocable<std::decay_t<Func> &>
		void dispatch( Func && p_function )
		{
			_DispatchedCall		   call( std::forward<Func>( p_function ) );
			const std::scoped_lock lock( _synchronizedCallsMutex );
			if ( not _shuttingDown )
			{
				_dispatchedCalls.emplace_back( std::move( call ) );
			}
		}

		/**
		 * @brief Updates the thread manager.
		 * Should be called in the main thread context.
		 */
		void update();

		/**
		 * @brief Returns the thread with the given id.
		 */
		BaseThread * get( const BaseThread::ID & ) noexcept;

		/**
		 * @brief Stops the thread with the given id.
		 */
		void stop( const BaseThread::ID & );

	  private:
		/**
		 * @brief Struct to store a synchronized call.
		 */
		struct _SynchronizedCall
		{
			_SynchronizedCall( const BaseThread::ID p_caller, std::packaged_task<void()> p_task ) :
				caller( p_caller ), task( std::move( p_task ) )
			{
			}

			BaseThread::ID			   caller;
			std::packaged_task<void()> task;
		};

		/**
		 * @brief Struct to store a dispatched call.
		 */
		struct _DispatchedCall
		{
			template<typename Func>
			_DispatchedCall( Func && p_function ) :
				task( std::forward<Func>( p_function ) ), result( task.get_future() )
			{
			}

			void execute()
			{
				task();
				result.get();
			}

			std::packaged_task<void()> task;
			std::future<void>		   result;
		};

		/**
		 * @brief List of threads.
		 */
		std::list<std::shared_ptr<BaseThread>> _threads;

		/**
		 * @brief Id of the thread that owns the ThreadManager.
		 */
		std::thread::id _ownerThreadId;

		/**
		 * @brief Mutex to synchronize access to the synchronized calls list.
		 */
		std::mutex _synchronizedCallsMutex;

		/**
		 * @brief List of synchronized calls to be executed.
		 */
		std::list<_SynchronizedCall> _synchronizedCalls;

		/**
		 * @brief List of dispatched calls to be executed.
		 */
		std::list<_DispatchedCall> _dispatchedCalls;

		/**
		 * @brief Set of thread ids that have been cancelled.
		 */
		std::unordered_set<BaseThread::ID> _cancelledCallers;

		/**
		 * @brief Is shutting down?
		 */
		bool _shuttingDown = false;

		/**
		 * @brief Default callbacks for threads created by this manager.
		 */
		BaseThread::ProgressCallback   _progressCallback;
		BaseThread::TerminatedCallback _terminatedCallback;

		/**
		 * @brief Cancels the synchronized calls for the given thread id.
		 */
		void _cancelSynchronizedCalls( const BaseThread::ID );

		/**
		 * @brief Cancels all synchronized calls.
		 */
		void _cancelSynchronizedCalls();
	};

} // namespace VTX::Util::Thread

#endif
