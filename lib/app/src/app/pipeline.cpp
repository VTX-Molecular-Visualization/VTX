#include "app/pipeline.hpp"
#include "app/action/action_manager.hpp"
#include "app/events.hpp"
#include "app/pass/pass_manager.hpp"
#include "app/services.hpp"
#include <renderer/renderer.hpp>
#include <util/event_hub.hpp>
#include <util/logger.hpp>
#include <util/monitoring/stats.hpp>
#include <util/thread/thread_manager.hpp>

namespace VTX::App
{
	void Pipeline::process( Util::Monitoring::Stats & p_stats, const float p_delta, const float p_elapsed )
	{
		using namespace Util;
		using namespace Events;

		Chrono					c;
		Monitoring::FrameInfo & frame	 = p_stats.newFrame();
		EventHub &				hub		 = HUB();
		bool					rendered = false;

		c.start();

		bool succeeded = _step( "Thread update", []() { VTX::App::THREAD().update(); } );

		// Trigger enqueued events.
		succeeded = _step( "Event update", [ &hub ]() { hub.update(); } ) && succeeded;

		// Process each step and record duration.
		succeeded = _step( "Pass update", [ p_delta, p_elapsed ]() { VTX::App::PASS().update( p_delta, p_elapsed ); } )
					&& succeeded;
		frame.set( UPDATE_PASSES, c.elapsedTime() );

		succeeded = _step( "Action update", [ p_delta, p_elapsed ]() { ACTION().update( p_delta, p_elapsed ); } )
					&& succeeded;

		succeeded
			= _step( "Update event", [ &hub, p_delta, p_elapsed ]() { hub.trigger<Update>( p_delta, p_elapsed ); } )
			  && succeeded;
		frame.set( UPDATE_EVENTS, c.elapsedTime() );

		succeeded
			= _step(
				  "Post update event", [ &hub, p_delta, p_elapsed ]() { hub.trigger<PostUpdate>( p_delta, p_elapsed ); }
			  )
			  && succeeded;
		frame.set( POST_UPDATE, c.elapsedTime() );

		succeeded
			= _step(
				  "Render",
				  [ &rendered, p_delta, p_elapsed ]() { rendered = VTX::App::RENDERER().render( p_delta, p_elapsed ); }
			  )
			  && succeeded;
		frame.set( RENDER, c.elapsedTime() );

		succeeded
			= _step(
				  "Post render event",
				  [ &hub, p_delta, p_elapsed, rendered ]() { hub.trigger<PostRender>( p_delta, p_elapsed, rendered ); }
			  )
			  && succeeded;
		frame.set( POST_RENDER, c.elapsedTime() );

		if ( succeeded )
		{
			_flushRepeatedError();
		}

		frame.set( LATE, c.elapsedTime() );
	}

	void Pipeline::_logError( const std::string_view p_step, const std::string_view p_error ) noexcept
	{
		try
		{
			std::string error = std::string( p_step ) + " failed: " + std::string( p_error );
			if ( error == _lastError )
			{
				++_lastErrorRepeatCount;
				return;
			}

			_flushRepeatedError();
			VTX_ERROR( "{}", error );
			_lastError = std::move( error );
		}
		catch ( ... )
		{
			VTX_ERROR( "{} failed", p_step );
		}
	}

	void Pipeline::_flushRepeatedError() noexcept
	{
		if ( _lastErrorRepeatCount > 0 )
		{
			VTX_ERROR( "Previous error repeated {} times: {}", _lastErrorRepeatCount, _lastError );
		}
		_lastErrorRepeatCount = 0;
		_lastError.clear();
	}
} // namespace VTX::App
