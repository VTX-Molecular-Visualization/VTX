#ifndef __VTX_APP_THREADING_MANAGER__
#define __VTX_APP_THREADING_MANAGER__

#include "app/threading/base_thread.hpp"
#include <list>
#include <memory>

namespace VTX::App::Threading
{
	/**
	 * @brief Thread manager.
	 * To spawn a thread call the createThread method passing a function that will be called in the context of the
	 * thread.
	 * Once the function returns the thread is put in the _stoppingThreads list and properly cleaned.
	 */
	class ThreadManager
	{
	  public:
		ThreadManager() {}
		ThreadManager( const ThreadManager & )			   = delete;
		ThreadManager & operator=( const ThreadManager & ) = delete;
		~ThreadManager();

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

		void get( const BaseThread::ID&, BaseThread *& ) noexcept;

	  private:
		std::list<std::shared_ptr<BaseThread>> _threads;
		std::list<std::shared_ptr<BaseThread>> _stoppingThreads;

		void _clearStoppedThreads();
	};

} // namespace VTX::App::Threading

#endif
