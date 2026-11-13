#include "app/pipeline.hpp"
#include "app/events.hpp"
#include "app/pass/pass_manager.hpp"
#include "app/pending_updates.hpp"
#include "app/services.hpp"
#include <renderer/renderer.hpp>
#include <util/event_hub.hpp>
#include <util/logger.hpp>
#include <util/thread/thread_manager.hpp>

namespace VTX::App
{
	void Pipeline::process( const E_PENDING_UPDATE p_pending, const float p_delta, const float p_elapsed )
	{
		using namespace Util;
		using namespace Events;

		EventHub & hub		 = HUB();
		bool	   rendered	 = false;
		bool	   succeeded = true;

		if ( Enum::hasAnyBit( p_pending, E_PENDING_UPDATE::THREADS ) )
		{
			succeeded = _step( "Thread update", []() { VTX::App::THREAD().update(); } );
		}

		// Trigger enqueued events.
		if ( Enum::hasAnyBit( p_pending, E_PENDING_UPDATE::EVENTS ) )
		{
			succeeded = _step( "Event update", [ &hub ]() { hub.update(); } ) && succeeded;
		}

		// Process each step and record duration.
		succeeded
			= _step(
				  "Pass update",
				  [ p_pending, p_delta, p_elapsed ]() { VTX::App::PASS().update( p_pending, p_delta, p_elapsed ); }
			  )
			  && succeeded;

		succeeded
			= _step( "Update event", [ &hub, p_delta, p_elapsed ]() { hub.trigger<Update>( p_delta, p_elapsed ); } )
			  && succeeded;

		succeeded
			= _step(
				  "Post update event", [ &hub, p_delta, p_elapsed ]() { hub.trigger<PostUpdate>( p_delta, p_elapsed ); }
			  )
			  && succeeded;

		succeeded
			= _step(
				  "Render",
				  [ &rendered, p_delta, p_elapsed ]() { rendered = VTX::App::RENDERER().render( p_delta, p_elapsed ); }
			  )
			  && succeeded;

		succeeded = _step(
						"Post render event",
						[ &hub, p_delta, p_elapsed, rendered ]()
						{ hub.trigger<PostRender>( p_delta, p_elapsed, rendered, not PENDING_UPDATES().any() ); }
					)
					&& succeeded;

		if ( succeeded )
		{
			_flushRepeatedError();
		}
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
