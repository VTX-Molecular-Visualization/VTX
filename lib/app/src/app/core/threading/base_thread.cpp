#include "app/core/threading/base_thread.hpp"
#include "app/core/threading/threading_system.hpp"
#include <util/math.hpp>

namespace VTX::App::Core::Threading
{
	BaseThread::~BaseThread()
	{
		if ( _thread.joinable() )
			_thread.detach();
	}

	void BaseThread::start( const AsyncOp & p_function )
	{
		_thread = std::jthread(
			[ this, p_function ]( std::stop_token p_stopToken )
			{
				p_function( p_stopToken, *this );
				_finish();
			}
		);
	}
	void BaseThread::start( const AsyncOp & p_function, const EndCallback & p_callback )
	{
		_thread = std::jthread(
			[ this, p_function, p_callback ]( std::stop_token p_stopToken )
			{
				const uint res = p_function( p_stopToken, *this );

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
			_thread.detach();

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

} // namespace VTX::App::Core::Threading
