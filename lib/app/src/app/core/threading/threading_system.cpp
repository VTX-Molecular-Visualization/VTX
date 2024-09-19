#include "app/core/threading/threading_system.hpp"

namespace VTX::App::Core::Threading
{
	ThreadingSystem::~ThreadingSystem()
	{
		for ( std::shared_ptr<BaseThread> & p_threadPtr : _threads )
		{
			p_threadPtr->stop();
		}

		_threads.clear();
		_stoppingThreads.clear();
	}

	BaseThread & ThreadingSystem::createThread( const BaseThread::AsyncOp & p_asyncOp )
	{
		std::shared_ptr<BaseThread> threadPtr = std::make_shared<BaseThread>( *this );
		_threads.emplace_back( threadPtr );

		threadPtr->start( p_asyncOp );

		return *threadPtr;
	}
	BaseThread & ThreadingSystem::createThread(
		const BaseThread::AsyncOp &		p_asyncOp,
		const BaseThread::EndCallback & p_callback
	)
	{
		std::shared_ptr<BaseThread> threadPtr = std::make_shared<BaseThread>( *this );
		_threads.emplace_back( threadPtr );

		threadPtr->start( p_asyncOp, p_callback );

		return *threadPtr;
	}

	void ThreadingSystem::lateUpdate() { _clearStoppedThreads(); }

	void ThreadingSystem::_killThread( const BaseThread & p_thread )
	{
		const std::list<std::shared_ptr<BaseThread>>::const_iterator it = _findPtrFromThread( p_thread );

		if ( it != _threads.end() )
		{
			_stoppingThreads.emplace_back( *it );
			_threads.erase( it );
		}
	}

	void ThreadingSystem::_clearStoppedThreads() { _stoppingThreads.clear(); }

	std::list<std::shared_ptr<BaseThread>>::const_iterator ThreadingSystem::_findPtrFromThread(
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
} // namespace VTX::App::Core::Threading
