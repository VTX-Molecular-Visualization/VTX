#include "app/pipeline.hpp"
#include "app/events.hpp"

namespace
{
	template<class Ev>
	void _processStep( VTX::Util::EventHub & p_hub, const float p_delta, const float p_elapsed )
	{
		p_hub.trigger<Ev>( p_delta, p_elapsed );
	}

	void _process(
		VTX::Util::Monitoring::FrameInfo & p_frame,
		VTX::Util::EventHub &			   p_hub,
		const float						   p_delta,
		const float						   p_elapsed
	)
	{
		using namespace VTX::Util;

		// Update.
		p_frame.set(
			VTX::App::Pipeline::UPDATE, CHRONO_CPU( &_processStep<VTX::App::Events::Update>, p_hub, p_delta, p_delta )
		);
		// Post update.
		p_frame.set(
			VTX::App::Pipeline::POST_UPDATE,
			CHRONO_CPU( &_processStep<VTX::App::Events::PostUpdate>, p_hub, p_delta, p_elapsed )
		);
		// Render.
		p_frame.set(
			VTX::App::Pipeline::RENDER, CHRONO_CPU( &_processStep<VTX::App::Events::Render>, p_hub, p_delta, p_elapsed )
		);
		// Post render.
		p_frame.set(
			VTX::App::Pipeline::POST_RENDER,
			CHRONO_CPU( &_processStep<VTX::App::Events::PostRender>, p_hub, p_delta, p_elapsed )
		);

		// Trigger enqueued events.
		p_hub.update();
	}
} // namespace

namespace VTX::App
{
	void Pipeline::process( Util::Monitoring::Stats & p_stats, const float p_delta, const float p_elapsed )
	{
		using namespace Util;

		Monitoring::FrameInfo & frameInfo = p_stats.newFrame();
		EventHub &				hub		  = HUB();

		// Main process.
		frameInfo.set( TICK_RATE, CHRONO_CPU( &_process, frameInfo, hub, p_delta, p_elapsed ) );
	}
} // namespace VTX::App
