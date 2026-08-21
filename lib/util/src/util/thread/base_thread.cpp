#include "util/thread/base_thread.hpp"
#include "util/thread/thread_manager.hpp"
#include <util/logger.hpp>
#include <util/math.hpp>
#include <utility>

namespace VTX::Util::Thread
{
	BaseThread::BaseThread( ThreadManager & p_manager ) :
		_manager( p_manager ), _progressCallback( p_manager._progressCallback ),
		_terminatedCallback( p_manager._terminatedCallback )
	{
	}

	BaseThread::~BaseThread()
	{
		if ( _thread.joinable() )
		{
			_thread.request_stop();
			_thread.join();
		}
	}

	void BaseThread::_start( const AsyncOp & p_function, const EndCallback & p_callback )
	{
		_thread = std::jthread(
			[ this, p_function, p_callback ]( std::stop_token p_stopToken )
			{
				uint res = 0;
				try
				{
					res = p_function( std::move( p_stopToken ), *this );
					if ( p_callback )
					{
						p_callback( *this, res, _stopped.load() );
					}
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "Unhandled exception in thread: {}", p_e.what() );
				}
				_finished.store( true, std::memory_order_release );
			}
		);
		_id = _thread.get_id();
	}

	void BaseThread::wait()
	{
		if ( _thread.joinable() )
		{
			_thread.join();
		}
	}

	void BaseThread::stop()
	{
		_stopped.store( true );
		_manager._cancelSynchronizedCalls( _id );
		if ( _thread.joinable() )
		{
			_thread.request_stop();
		}
	}

	void BaseThread::setProgress( const float p_value )
	{
		const float clampedValue = Util::Math::clamp( p_value, 0.f, 1.f );
		_progress.store( clampedValue, std::memory_order_relaxed );
	}

	std::string BaseThread::getProgressText() const
	{
		std::lock_guard lock( _progressTextMutex );
		return _progressText;
	}

	void BaseThread::setProgressText( const std::string & p_text )
	{
		std::lock_guard lock( _progressTextMutex );
		_progressText = p_text;
	}

	void BaseThread::setProgressCallback( ProgressCallback p_callback ) { _progressCallback = std::move( p_callback ); }

	void BaseThread::setTerminatedCallback( TerminatedCallback p_callback )
	{ _terminatedCallback = std::move( p_callback ); }

} // namespace VTX::Util::Thread
