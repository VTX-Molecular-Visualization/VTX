#include "app/threading/thread_manager.hpp"
#include "app/events.hpp"
#include "app/services.hpp"
#include <util/event_hub.hpp>

namespace VTX::App::Threading
{
	ThreadManager::~ThreadManager()
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
			ThreadManager *							 p_system,
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
			ThreadManager *							 p_system,
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
	BaseThread & ThreadManager::createThread( const BaseThread::AsyncOp & p_asyncOp )
	{
		return _createThread( this, _threads, p_asyncOp );
	}
	BaseThread & ThreadManager::createThread( const BaseThread::StoppableAsyncOp & p_asyncOp )
	{
		return _createThread( this, _threads, p_asyncOp );
	}
	BaseThread & ThreadManager::createThread(
		const BaseThread::AsyncOp &		p_asyncOp,
		const BaseThread::EndCallback & p_callback
	)
	{
		return _createThread( this, _threads, p_asyncOp, p_callback );
	}

	BaseThread & ThreadManager::createThread(
		const BaseThread::StoppableAsyncOp & p_asyncOp,
		const BaseThread::EndCallback &		 p_callback
	)
	{
		return _createThread( this, _threads, p_asyncOp, p_callback );
	}

	void ThreadManager::lateUpdate()
	{
		for ( const std::shared_ptr<BaseThread> & thread : _threads )
		{
			HUB().trigger<Events::ThreadProgress>( thread->getId(), thread->getProgress(), thread->getProgressText() );

			if ( thread->isFinished() )
			{
				HUB().trigger<Events::ThreadTerminated>( thread->getId(), thread->isManuallyStopped() );
				_stoppingThreads.emplace_back( thread );
			}
		}

		_threads.remove_if( []( const std::shared_ptr<BaseThread> & p_thread ) { return p_thread->isFinished(); } );

		_clearStoppedThreads();
	}

	void ThreadManager::get( const BaseThread::ID & p_id, BaseThread *& p_out ) noexcept
	{
		p_out = nullptr;
		for ( auto & it_thrptr : _threads )
			if ( it_thrptr->getId() == p_id )
			{
				p_out = it_thrptr.get();
				return;
			}
		for ( auto & it_thrptr : _stoppingThreads )
			if ( it_thrptr->getId() == p_id )
			{
				p_out = it_thrptr.get();
				return;
			}
	}

	void ThreadManager::_clearStoppedThreads() { _stoppingThreads.clear(); }
} // namespace VTX::App::Threading
