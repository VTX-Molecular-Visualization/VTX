#include "app/pipeline.hpp"
#include "app/events.hpp"
#include "app/pass/pass_manager.hpp"
#include "app/services.hpp"
#include "app/threading/thread_manager.hpp"
#include <renderer/renderer.hpp>
#include <util/event_hub.hpp>
#include <util/monitoring/stats.hpp>

namespace
{
	template<class Ev>
	void _processStep( VTX::Util::EventHub & p_hub, const float p_delta, const float p_elapsed )
	{
		p_hub.trigger<Ev>( p_delta, p_elapsed );
	}

	inline void _passes( const float p_delta, const float p_elapsed ) { VTX::App::PASS().update( p_delta, p_elapsed ); }

	inline void _render( const float p_delta, const float p_elapsed ) noexcept
	{
		VTX::App::RENDERER().render( p_delta, p_elapsed );
	}

	inline void _threadLateUpdate() { VTX::App::THREAD().lateUpdate(); }

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
		frame.set( UPDATE_PASSES, CHRONO_CPU( &_passes, p_delta, p_elapsed ) );
		frame.set( UPDATE_EVENTS, CHRONO_CPU( &_processStep<Update>, hub, p_delta, p_elapsed ) );
		frame.set( POST_UPDATE, CHRONO_CPU( &_processStep<PostUpdate>, hub, p_delta, p_elapsed ) );
		frame.set( RENDER, CHRONO_CPU( &_render, p_delta, p_elapsed ) );
		frame.set( POST_RENDER, CHRONO_CPU( &_processStep<PostRender>, hub, p_delta, p_elapsed ) );
		frame.set( LATE, CHRONO_CPU( &_threadLateUpdate ) );
	}
} // namespace VTX::App
