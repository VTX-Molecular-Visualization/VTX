#ifndef __VTX_APP_CORE_THREADING_MANAGER__
#define __VTX_APP_CORE_THREADING_MANAGER__

#include "app/core/threading/base_thread.hpp"
#include <list>
#include <memory>
#include <util/singleton.hpp>

namespace VTX::App::Core::Threading
{
	/**
	 * @brief Thread manager.
	 * To spawn a thread call the createThread method passing a function that will be called in the context of the
	 * thread.
	 * Once the function returns the thread is put in the _stoppingThreads list and properly cleaned.
	 */
	class ThreadingSystem
	{
		friend class BaseThread;

	  public:
		ThreadingSystem() {}
		ThreadingSystem( const ThreadingSystem & )			   = delete;
		ThreadingSystem & operator=( const ThreadingSystem & ) = delete;
		~ThreadingSystem();

		/*
		 * @brief Spawn a thread and executes the content of p_asyncOp in the new thread context.
		 */
		BaseThread & createThread( const BaseThread::AsyncOp & p_asyncOp );
		BaseThread & createThread( const BaseThread::StoppableAsyncOp & p_asyncOp );
		/*
		 * @brief Spawn a thread and executes the content of p_asyncOp in the new thread context.
		 * Once the p_asyncOp has returned, p_callback is executed.
		 */
		BaseThread & createThread( const BaseThread::AsyncOp & p_asyncOp, const BaseThread::EndCallback & p_callback );
		BaseThread & createThread(
			const BaseThread::StoppableAsyncOp & p_asyncOp,
			const BaseThread::EndCallback &		 p_callback
		);

		void lateUpdate();

	  private:
		std::list<std::shared_ptr<BaseThread>> _threads;
		std::list<std::shared_ptr<BaseThread>> _stoppingThreads;

		/*
		 * @brief Called by an ending thread so that the manager can erase the thread from the _threads member and put
		 * it in the _stoppingThreads member.
		 */
		void _killThread( const BaseThread & p_thread );
		/**
		 * @brief Clear _stoppingThreads member.
		 */
		void _clearStoppedThreads();

		std::list<std::shared_ptr<BaseThread>>::const_iterator _findPtrFromThread( const BaseThread & p_thread ) const;
	};

} // namespace VTX::App::Core::Threading

namespace VTX::App
{
	// Access to the worker manager class in order to launch thread.
	inline Core::Threading::ThreadingSystem & THREADING_SYSTEM()
	{
		return Util::Singleton<Core::Threading::ThreadingSystem>::get();
	}
} // namespace VTX::App

#endif
