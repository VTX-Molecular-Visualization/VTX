#include "app/core/worker/base_thread.hpp"
#include <util/math.hpp>

namespace VTX::App::Core::Worker
{
	BaseThread::~BaseThread() { _thread.request_stop(); }

	void BaseThread::run( const AsyncOp & p_asyncOp )
	{
		_thread = std::jthread( [ this, p_asyncOp ]() { p_asyncOp( *this ); } );
	}
	void BaseThread::run( const AsyncOp & p_asyncOp, const EndCallback & p_callback )
	{
		_thread = std::jthread(
			[ this, p_asyncOp, p_callback ]()
			{
				const uint retCode = p_asyncOp( *this );
				if ( p_callback )
					p_callback( *this, retCode );
			}
		);
	}

	void BaseThread::wait() { _thread.join(); }
	void BaseThread::stop() { _thread.request_stop(); }

	bool BaseThread::isFinished() const { return _thread.joinable(); }

	void BaseThread::setProgress( const float p_value )
	{
		const float clampedValue = Util::Math::clamp( p_value, 0.f, 1.f );

		if ( _progress != clampedValue )
		{
			_progress = clampedValue;
			progressCallback.call( _progress );
		}
	}

} // namespace VTX::App::Core::Worker
