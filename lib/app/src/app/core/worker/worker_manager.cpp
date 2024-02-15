#include "app/core/worker/worker_manager.hpp"

namespace VTX::App::Core::Worker
{
	WorkerManager::~WorkerManager()
	{
		for ( std::shared_ptr<BaseThread> & p_threadPtr : _threads )
		{
			p_threadPtr->stop();
		}

		_threads.clear();
		_stoppingThreads.clear();
	}

	BaseThread & WorkerManager::createThread( const BaseThread::AsyncOp & p_asyncOp )
	{
		std::shared_ptr<BaseThread> threadPtr = std::make_shared<BaseThread>( *this );
		_threads.emplace_back( threadPtr );

		threadPtr->start( p_asyncOp );

		return *threadPtr;
	}
	BaseThread & WorkerManager::createThread(
		const BaseThread::AsyncOp &		p_asyncOp,
		const BaseThread::EndCallback & p_callback
	)
	{
		std::shared_ptr<BaseThread> threadPtr = std::make_shared<BaseThread>( *this );
		_threads.emplace_back( threadPtr );

		threadPtr->start( p_asyncOp, p_callback );

		return *threadPtr;
	}

	void WorkerManager::lateUpdate() { _clearStoppedThreads(); }

	void WorkerManager::_killThread( const BaseThread & p_thread )
	{
		const std::list<std::shared_ptr<BaseThread>>::const_iterator it = _findPtrFromThread( p_thread );

		if ( it != _threads.end() )
		{
			_stoppingThreads.emplace_back( *it );
			_threads.erase( it );
		}
	}

	void WorkerManager::_clearStoppedThreads() { _stoppingThreads.clear(); }

	std::list<std::shared_ptr<BaseThread>>::const_iterator WorkerManager::_findPtrFromThread(
		const BaseThread & p_thread
	) const
	{
		const BaseThread * const threadPtr = &p_thread;

		return std::find_if(
			_threads.cbegin(),
			_threads.cend(),
			[ threadPtr ]( const std::shared_ptr<BaseThread> & p_ptr ) { return threadPtr == p_ptr.get(); }
		);
	}
} // namespace VTX::App::Core::Worker
