#ifndef __VTX_APP_CORE_WORKER_MANAGER__
#define __VTX_APP_CORE_WORKER_MANAGER__

#include "app/core/worker/base_thread.hpp"
#include <list>
#include <memory>

namespace VTX::App::Core::Worker
{
	class WorkerManager
	{
		friend class BaseThread;

	  public:
		WorkerManager() {}
		WorkerManager( const WorkerManager & )			   = delete;
		WorkerManager & operator=( const WorkerManager & ) = delete;
		~WorkerManager();

		BaseThread & createThread( const BaseThread::AsyncOp & p_asyncOp );
		BaseThread & createThread( const BaseThread::AsyncOp & p_asyncOp, const BaseThread::EndCallback & p_callback );

		void lateUpdate();

	  private:
		std::list<std::shared_ptr<BaseThread>> _threads			= std::list<std::shared_ptr<BaseThread>>();
		std::list<std::shared_ptr<BaseThread>> _stoppingThreads = std::list<std::shared_ptr<BaseThread>>();

		void _killThread( const BaseThread & p_thread );
		void _clearStoppedThreads();

		std::list<std::shared_ptr<BaseThread>>::const_iterator _findPtrFromThread( const BaseThread & p_thread ) const;
	};

} // namespace VTX::App::Core::Worker
#endif
