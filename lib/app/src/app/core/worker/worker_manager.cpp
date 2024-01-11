#include "app/core/worker/worker_manager.hpp"

namespace VTX::App::Core::Worker
{
	BaseThread & WorkerManager::createThread( const BaseThread::AsyncOp & p_asyncOp )
	{
		BaseThread & thread = _threads.emplace_back();
		thread.run( p_asyncOp );

		return thread;
	}
	BaseThread & WorkerManager::createThread(
		const BaseThread::AsyncOp &		p_asyncOp,
		const BaseThread::EndCallback & p_callback
	)
	{
		BaseThread & thread = _threads.emplace_back();
		thread.run( p_asyncOp, p_callback );

		return thread;
	}

} // namespace VTX::App::Core::Worker
