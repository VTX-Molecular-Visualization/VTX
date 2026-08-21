#include "util/thread/thread_manager.hpp"
#include <exception>
#include <util/logger.hpp>
#include <utility>

namespace VTX::Util::Thread
{
	ThreadManager::ThreadManager() : _ownerThreadId( std::this_thread::get_id() ) {}

	void ThreadManager::setDefaultProgressCallback( BaseThread::ProgressCallback p_callback )
	{ _progressCallback = std::move( p_callback ); }

	void ThreadManager::setDefaultTerminatedCallback( BaseThread::TerminatedCallback p_callback )
	{ _terminatedCallback = std::move( p_callback ); }

	ThreadManager::~ThreadManager()
	{
		_cancelSynchronizedCalls();
		for ( const std::shared_ptr<BaseThread> & thread : _threads )
		{
			thread->stop();
		}
		for ( const std::shared_ptr<BaseThread> & thread : _threads )
		{
			thread->wait();
		}
		_threads.clear();
	}

	void ThreadManager::update()
	{
		const auto callThreadCallback
			= []( const BaseThread::ProgressCallback & p_callback, const BaseThread & p_thread )
		{
			if ( not p_callback )
			{
				return;
			}
			try
			{
				p_callback( p_thread );
			}
			catch ( const std::exception & p_exception )
			{
				VTX_ERROR( "Unhandled exception in thread callback: {}", p_exception.what() );
			}
			catch ( ... )
			{
				VTX_ERROR( "Unhandled unknown exception in thread callback" );
			}
		};

		std::list<_SynchronizedCall> calls;
		{
			const std::scoped_lock lock( _synchronizedCallsMutex );
			calls.splice( calls.end(), _synchronizedCalls );
		}

		for ( _SynchronizedCall & call : calls )
		{
			call.task();
		}

		/**
		 * @brief List of threads that are stopping.
		 */
		std::list<std::shared_ptr<BaseThread>> stoppingThreads;
		for ( auto threadIterator = _threads.begin(); threadIterator != _threads.end(); )
		{
			const std::shared_ptr<BaseThread> & thread = *threadIterator;
			// TODO: do not spam progress event every frame.
			callThreadCallback( thread->_progressCallback, *thread );
			if ( thread->isFinished() )
			{
				callThreadCallback( thread->_terminatedCallback, *thread );
				stoppingThreads.splice( stoppingThreads.end(), _threads, threadIterator++ );
			}
			else
			{
				++threadIterator;
			}
		}

		{
			const std::scoped_lock lock( _synchronizedCallsMutex );
			for ( const std::shared_ptr<BaseThread> & thread : stoppingThreads )
			{
				_cancelledCallers.erase( thread->getId() );
			}
		}
	}

	BaseThread & ThreadManager::createThread(
		const BaseThread::AsyncOp &		p_asyncOp,
		const BaseThread::EndCallback & p_callback
	)
	{
		std::shared_ptr<BaseThread> thread( new BaseThread( *this ) );
		_threads.emplace_back( thread );
		try
		{
			thread->_start( p_asyncOp, p_callback );
		}
		catch ( ... )
		{
			_threads.pop_back();
			throw;
		}
		return *thread;
	}

	BaseThread * ThreadManager::get( const BaseThread::ID & p_id ) noexcept
	{
		for ( auto & it_thrptr : _threads )
		{
			if ( it_thrptr->getId() == p_id )
			{
				return it_thrptr.get();
			}
		}
		return nullptr;
	}

	void ThreadManager::stop( const BaseThread::ID & p_id )
	{
		BaseThread * thread = get( p_id );
		if ( thread != nullptr )
		{
			thread->stop();
		}
	}

	void ThreadManager::_cancelSynchronizedCalls( const BaseThread::ID p_caller )
	{
		const std::scoped_lock lock( _synchronizedCallsMutex );
		if ( _shuttingDown )
		{
			return;
		}
		_cancelledCallers.emplace( p_caller );
		_synchronizedCalls.remove_if( [ p_caller ]( const _SynchronizedCall & p_call )
									  { return p_call.caller == p_caller; } );
	}

	void ThreadManager::_cancelSynchronizedCalls()
	{
		const std::scoped_lock lock( _synchronizedCallsMutex );
		_shuttingDown = true;
		_synchronizedCalls.clear();
	}
} // namespace VTX::Util::Thread
