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
	namespace
	{
		template<typename callable>
		BaseThread & _createThread(
			ThreadingSystem *						 p_system,
			std::list<std::shared_ptr<BaseThread>> & p_threadCollection,
			callable								 p_func
		)
		{
			std::shared_ptr<BaseThread> threadPtr = std::make_shared<BaseThread>( *p_system );
			p_threadCollection.emplace_back( threadPtr );

			threadPtr->start( p_func );

			return *threadPtr;
		}

		template<typename callable>
		BaseThread & _createThread(
			ThreadingSystem *						 p_system,
			std::list<std::shared_ptr<BaseThread>> & p_threadCollection,
			callable								 p_asyncOp,
			const BaseThread::EndCallback &			 p_callback
		)
		{
			std::shared_ptr<BaseThread> threadPtr = std::make_shared<BaseThread>( *p_system );
			p_threadCollection.emplace_back( threadPtr );

			threadPtr->start( p_asyncOp, p_callback );

			return *threadPtr;
		}
	} // namespace
	BaseThread & ThreadingSystem::createThread( const BaseThread::AsyncOp & p_asyncOp )
	{
		return _createThread( this, _threads, p_asyncOp );
	}
	BaseThread & ThreadingSystem::createThread( const BaseThread::StoppableAsyncOp & p_asyncOp )
	{
		return _createThread( this, _threads, p_asyncOp );
	}
	BaseThread & ThreadingSystem::createThread(
		const BaseThread::AsyncOp &		p_asyncOp,
		const BaseThread::EndCallback & p_callback
	)
	{
		return _createThread( this, _threads, p_asyncOp, p_callback );
	}

	BaseThread & ThreadingSystem::createThread(
		const BaseThread::StoppableAsyncOp & p_asyncOp,
		const BaseThread::EndCallback &		 p_callback
	)
	{
		return _createThread( this, _threads, p_asyncOp, p_callback );
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
