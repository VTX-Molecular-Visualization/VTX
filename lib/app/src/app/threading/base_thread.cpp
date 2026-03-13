#include "app/threading/base_thread.hpp"
#include "app/threading/thread_manager.hpp"
#include <util/logger.hpp>
#include <util/math.hpp>

namespace VTX::App::Threading
{
	BaseThread::~BaseThread()
	{
		if ( _thread.joinable() )
			_thread.detach();
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

				if ( _stopped )
				{
					p_callback( *this, res );
				}

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

				if ( _stopped )
				{
					p_callback( *this, res );
				}

				_finish();
			}
		);
	}

	void BaseThread::wait()
	{
		if ( _thread.joinable() )
			_thread.join();
	}
	void BaseThread::stop()
	{
		onProgress.clear();

		if ( _thread.joinable() )
			_thread.request_stop();

		_stopped = true;
	}
	bool BaseThread::isFinished() const { return !_thread.joinable(); }

	void BaseThread::setProgress( const float p_value )
	{
		const float clampedValue = Util::Math::clamp( p_value, 0.f, 1.f );

		if ( _progress != clampedValue )
		{
			_progress = clampedValue;
			onProgress( _progress );
		}
	}

	void BaseThread::_finish() { _manager._killThread( *this ); }

} // namespace VTX::App::Threading
