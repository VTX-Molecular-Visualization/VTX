#ifndef __VTX_APP_INTERNAL_MONITORING_ALL_METRICS__
#define __VTX_APP_INTERNAL_MONITORING_ALL_METRICS__

#include "app/core/monitoring/frame_info.hpp"

namespace VTX::App::Internal::Monitoring
{
	const size_t TICK_RATE_KEY		 = Core::Monitoring::FrameInfo::hash( "TICK_RATE_KEY" );
	const size_t DRAW_CALL_COUNT_KEY = Core::Monitoring::FrameInfo::hash( "DRAW_CALL_COUNT" );

	const size_t PRE_UPDATE_DURATION_KEY  = Core::Monitoring::FrameInfo::hash( "PRE_UPDATE_DURATION" );
	const size_t UPDATE_DURATION_KEY	  = Core::Monitoring::FrameInfo::hash( "UPDATE_DURATION" );
	const size_t LATE_UPDATE_DURATION_KEY = Core::Monitoring::FrameInfo::hash( "LATE_UPDATE_DURATION" );
	const size_t POST_UPDATE_DURATION_KEY = Core::Monitoring::FrameInfo::hash( "POST_UPDATE_DURATION" );

	const size_t PRE_RENDER_DURATION_KEY  = Core::Monitoring::FrameInfo::hash( "PRE_RENDER_DURATION" );
	const size_t RENDER_DURATION_KEY	  = Core::Monitoring::FrameInfo::hash( "RENDER_DURATION" );
	const size_t POST_RENDER_DURATION_KEY = Core::Monitoring::FrameInfo::hash( "POST_RENDER_DURATION" );
} // namespace VTX::App::Internal::Monitoring
#endif
