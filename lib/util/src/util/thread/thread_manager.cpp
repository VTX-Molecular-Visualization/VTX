#include "util/thread/thread_manager.hpp"
#include <algorithm>
#include <exception>
#include <iterator>
#include <util/logger.hpp>
#include <utility>
#include <vector>

namespace VTX::Util::Thread
{
	ThreadManager::ThreadManager() : _ownerThreadId( std::this_thread::get_id() ) {}

	void ThreadManager::setDefaultProgressCallback( BaseThread::ProgressCallback p_callback )
	{ _progressCallback = std::move( p_callback ); }

	void ThreadManager::setDefaultTerminatedCallback( BaseThread::TerminatedCallback p_callback )
	{ _terminatedCallback = std::move( p_callback ); }

	ThreadManager::~ThreadManager()
	{
		try
		{
			_cancelSynchronizedCalls();
		}
		catch ( ... )
		{
			LOGGER::logException( "Failed to cancel synchronized calls" );
		}
		std::vector<std::shared_ptr<BaseThread>> threads;
		try
		{
			const std::scoped_lock lock( _threadsMutex );
			threads.assign( std::make_move_iterator( _threads.begin() ), std::make_move_iterator( _threads.end() ) );
			_threads.clear();
		}
		catch ( ... )
		{
			LOGGER::logException( "Failed to collect threads" );
		}
		for ( const std::shared_ptr<BaseThread> & thread : threads )
		{
			try
			{
				thread->stop();
			}
			catch ( ... )
			{
				LOGGER::logException( "Failed to stop thread" );
			}
		}
		for ( const std::shared_ptr<BaseThread> & thread : threads )
		{
			try
			{
				thread->wait();
			}
			catch ( ... )
			{
				LOGGER::logException( "Failed to wait for thread" );
			}
		}
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

		std::list<SynchronizedCall> calls;
		std::list<DispatchedCall>	dispatchedCalls;
		{
			const std::scoped_lock lock( _synchronizedCallsMutex );
			calls.splice( calls.end(), _synchronizedCalls );
			dispatchedCalls.splice( dispatchedCalls.end(), _dispatchedCalls );
		}

		for ( SynchronizedCall & call : calls )
		{
			call.task();
		}
		for ( DispatchedCall & call : dispatchedCalls )
		{
			try
			{
				call.execute();
			}
			catch ( const std::exception & p_exception )
			{
				VTX_ERROR( "Unhandled exception in dispatched call: {}", p_exception.what() );
			}
			catch ( ... )
			{
				VTX_ERROR( "Unhandled unknown exception in dispatched call" );
			}
		}

		/**
		 * @brief List of threads that are stopping.
		 */
		std::list<std::shared_ptr<BaseThread>>	 stoppingThreads;
		std::vector<std::shared_ptr<BaseThread>> threads;
		{
			const std::scoped_lock lock( _threadsMutex );
			threads.assign( _threads.begin(), _threads.end() );
		}
		for ( const std::shared_ptr<BaseThread> & thread : threads )
		{
			// TODO: do not spam progress event every frame.
			callThreadCallback( thread->_progressCallback, *thread );
			if ( thread->isFinished() )
			{
				callThreadCallback( thread->_terminatedCallback, *thread );
				stoppingThreads.emplace_back( thread );
			}
		}
		if ( not stoppingThreads.empty() )
		{
			const std::scoped_lock lock( _threadsMutex );
			_threads.remove_if( [ &stoppingThreads ]( const std::shared_ptr<BaseThread> & p_thread )
								{ return std::ranges::find( stoppingThreads, p_thread ) != stoppingThreads.end(); } );
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
		const std::shared_ptr<BaseThread> thread( new BaseThread( *this ) );
		thread->_start( p_asyncOp, p_callback );
		{
			const std::scoped_lock lock( _threadsMutex );
			_threads.emplace_back( thread );
		}
		_onPending();
		return *thread;
	}

	BaseThread * ThreadManager::get( const ID & p_id )
	{
		const std::scoped_lock lock( _threadsMutex );
		for ( auto & it_thrptr : _threads )
		{
			if ( it_thrptr->getId() == p_id )
			{
				return it_thrptr.get();
			}
		}
		return nullptr;
	}

	void ThreadManager::stop( const ID & p_id )
	{
		std::shared_ptr<BaseThread> thread;
		{
			const std::scoped_lock lock( _threadsMutex );
			const auto			   it = std::ranges::find_if(
				_threads,
				[ &p_id ]( const std::shared_ptr<BaseThread> & p_thread ) { return p_thread->getId() == p_id; }
			);
			if ( it != _threads.end() )
			{
				thread = *it;
			}
		}
		if ( thread != nullptr )
		{
			thread->stop();
		}
	}

	void ThreadManager::_cancelSynchronizedCalls( const ID p_caller )
	{
		const std::scoped_lock lock( _synchronizedCallsMutex );
		if ( _shuttingDown )
		{
			return;
		}
		_cancelledCallers.emplace( p_caller );
		_synchronizedCalls.remove_if( [ p_caller ]( const SynchronizedCall & p_call )
									  { return p_call.caller == p_caller; } );
	}

	void ThreadManager::_cancelSynchronizedCalls()
	{
		const std::scoped_lock lock( _synchronizedCallsMutex );
		_shuttingDown = true;
		_synchronizedCalls.clear();
		_dispatchedCalls.clear();
	}
} // namespace VTX::Util::Thread
