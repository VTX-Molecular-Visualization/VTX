#ifndef __VTX_APP_PIPELINE__
#define __VTX_APP_PIPELINE__

#include "app/event_hub.hpp"
#include "app/events.hpp"
#include <util/logger.hpp>

namespace VTX::App
{

	struct Pipeline
	{
		inline void update( const float p_delta, const float p_elapsed )
		{
			EventHub & hub = HUB();
			hub.trigger<Events::FrameStarted>();
			hub.trigger<Events::PreUpdate>( p_delta, p_elapsed );
			hub.trigger<Events::Update>( p_delta, p_elapsed );
			hub.update();
			hub.trigger<Events::PostUpdate>( p_delta, p_elapsed );
			hub.trigger<Events::Render>( p_delta, p_elapsed );
			hub.trigger<Events::PostRender>( p_delta, p_elapsed );
			hub.trigger<Events::FrameEnded>();
		}
	};

} // namespace VTX::App

#endif
