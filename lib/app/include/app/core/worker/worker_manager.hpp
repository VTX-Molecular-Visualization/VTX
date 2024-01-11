#ifndef __VTX_APP_CORE_WORKER_MANAGER__
#define __VTX_APP_CORE_WORKER_MANAGER__

#include "app/core/worker/base_thread.hpp"
#include <list>

// TODO reimplemente that without Qt
namespace VTX::App::Core::Worker
{
	class WorkerManager final
	{
	  public:
		WorkerManager() {}
		WorkerManager( const WorkerManager & )			   = delete;
		WorkerManager & operator=( const WorkerManager & ) = delete;
		~WorkerManager() {}

		BaseThread & createThread( const BaseThread::AsyncOp & p_asyncOp );
		BaseThread & createThread( const BaseThread::AsyncOp & p_asyncOp, const BaseThread::EndCallback & p_callback );

	  private:
		std::list<BaseThread> _threads = std::list<BaseThread>();
	};

} // namespace VTX::App::Core::Worker
#endif
