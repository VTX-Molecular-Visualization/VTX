#include "app/pipeline.hpp"
#include "app/events.hpp"
#include "app/services.hpp"
#include <util/event_hub.hpp>
#include <util/monitoring/stats.hpp>

namespace
{
	template<class Ev>
	void _processStep( VTX::Util::EventHub & p_hub, const float p_delta, const float p_elapsed )
	{
		p_hub.trigger<Ev>( p_delta, p_elapsed );
	}

} // namespace

namespace VTX::App
{
	void Pipeline::process( Util::Monitoring::Stats & p_stats, const float p_delta, const float p_elapsed )
	{
		using namespace Util;
		using namespace Events;

		Monitoring::FrameInfo & frame = p_stats.newFrame();
		EventHub &				hub	  = HUB();

		// Trigger enqueued events.
		hub.update();

		// Process each step and record duration.
		// frame.set( UPDATE, CHRONO_CPU( &_processStep<Update>, hub, p_delta, p_delta ) );
		frame.set( POST_UPDATE, CHRONO_CPU( &_processStep<PostUpdate>, hub, p_delta, p_elapsed ) );
		frame.set( RENDER, CHRONO_CPU( &_processStep<Render>, hub, p_delta, p_elapsed ) );
		frame.set( POST_RENDER, CHRONO_CPU( &_processStep<PostRender>, hub, p_delta, p_elapsed ) );
	}
} // namespace VTX::App
