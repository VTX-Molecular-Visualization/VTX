#include "app/core/worker/base_thread.hpp"
#include "app/core/worker/worker_manager.hpp"
#include <util/math.hpp>

namespace VTX::App::Core::Worker
{
	BaseThread::~BaseThread()
	{
		if ( _thread.joinable() )
			_thread.detach();
	}

	void BaseThread::start( const AsyncOp & p_function )
	{
		_thread = std::thread(
			[ this, p_function ]()
			{
				p_function( *this );
				_finish();
			}
		);
	}
	void BaseThread::start( const AsyncOp & p_function, const EndCallback & p_callback )
	{
		_thread = std::thread(
			[ this, p_function, p_callback ]()
			{
				const uint res = p_function( *this );

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
		progressCallback.clear();

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
			progressCallback.call( _progress );
		}
	}

	void BaseThread::_finish() { _manager._killThread( *this ); }

} // namespace VTX::App::Core::Worker
