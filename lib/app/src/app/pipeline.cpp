#include "app/pipeline.hpp"
#include "app/action/action_manager.hpp"
#include "app/events.hpp"
#include "app/pass/pass_manager.hpp"
#include "app/services.hpp"
#include <renderer/renderer.hpp>
#include <util/event_hub.hpp>
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

		VTX::App::THREAD().update();

		// Trigger enqueued events.
		hub.update();

		// Process each step and record duration.
		VTX::App::PASS().update( p_delta, p_elapsed );
		frame.set( UPDATE_PASSES, c.elapsedTime() );

		ACTION().update( p_delta, p_elapsed );

		hub.trigger<Update>( p_delta, p_elapsed );
		frame.set( UPDATE_EVENTS, c.elapsedTime() );

		hub.trigger<PostUpdate>( p_delta, p_elapsed );
		frame.set( POST_UPDATE, c.elapsedTime() );

		rendered = VTX::App::RENDERER().render( p_delta, p_elapsed );
		frame.set( RENDER, c.elapsedTime() );

		hub.trigger<PostRender>( p_delta, p_elapsed, rendered );
		frame.set( POST_RENDER, c.elapsedTime() );

		frame.set( LATE, c.elapsedTime() );
	}
} // namespace VTX::App
