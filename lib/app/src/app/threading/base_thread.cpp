#include "app/threading/base_thread.hpp"
#include "app/threading/thread_manager.hpp"
#include <util/logger.hpp>
#include <util/math.hpp>

namespace VTX::App::Threading
{
	BaseThread::~BaseThread()
	{
		if ( _thread.joinable() )
		{
			_thread.request_stop();
			_thread.join();
		}
	}

	void BaseThread::start( const AsyncOp & p_function )
	{
		_thread = std::jthread(
			[ this, p_function ]()
			{
				try
				{
					p_function( *this );
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "Unhandled exception in thread: {}", p_e.what() );
				}
				_finish();
			}
		);
	}

	void BaseThread::start( const StoppableAsyncOp & p_function )
	{
		_thread = std::jthread(
			[ this, p_function ]( std::stop_token p_stopToken )
			{
				try
				{
					p_function( std::move( p_stopToken ), *this );
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "Unhandled exception in thread: {}", p_e.what() );
				}
				_finish();
			}
		);
	}

	void BaseThread::start( const AsyncOp & p_function, const EndCallback & p_callback )
	{
		_thread = std::jthread(
			[ this, p_function, p_callback ]()
			{
				uint res = 0;
				try
				{
					res = p_function( *this );
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "Unhandled exception in thread: {}", p_e.what() );
				}

				p_callback( *this, res, _stopped );
				_finish();
			}
		);
	}

	void BaseThread::start( const StoppableAsyncOp & p_function, const EndCallback & p_callback )
	{
		_thread = std::jthread(
			[ this, p_function, p_callback ]( std::stop_token p_stopToken )
			{
				uint res = 0;
				try
				{
					res = p_function( std::move( p_stopToken ), *this );
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "Unhandled exception in thread: {}", p_e.what() );
				}

				p_callback( *this, res, _stopped );
				_finish();
			}
		);
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
		if ( _thread.joinable() )
		{
			_thread.request_stop();
		}

		_stopped = true;
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

	void BaseThread::_finish() { _finished.store( true, std::memory_order_relaxed ); }

} // namespace VTX::App::Threading
