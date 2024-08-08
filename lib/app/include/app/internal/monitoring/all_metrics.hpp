#ifndef __VTX_APP_INTERNAL_MONITORING_ALL_METRICS__
#define __VTX_APP_INTERNAL_MONITORING_ALL_METRICS__

#include "app/core/monitoring/frame_info.hpp"
#include <util/hashing.hpp>

namespace VTX::App::Internal::Monitoring
{
	const Hash TICK_RATE_KEY	   = Util::hash( "TICK_RATE_KEY" );
	const Hash DRAW_CALL_COUNT_KEY = Util::hash( "DRAW_CALL_COUNT" );

	const Hash PRE_UPDATE_DURATION_KEY	= Util::hash( "PRE_UPDATE_DURATION" );
	const Hash UPDATE_DURATION_KEY		= Util::hash( "UPDATE_DURATION" );
	const Hash LATE_UPDATE_DURATION_KEY = Util::hash( "LATE_UPDATE_DURATION" );
	const Hash POST_UPDATE_DURATION_KEY = Util::hash( "POST_UPDATE_DURATION" );

	const Hash PRE_RENDER_DURATION_KEY	= Util::hash( "PRE_RENDER_DURATION" );
	const Hash RENDER_DURATION_KEY		= Util::hash( "RENDER_DURATION" );
	const Hash POST_RENDER_DURATION_KEY = Util::hash( "POST_RENDER_DURATION" );

	const Hash END_OF_FRAME_ONE_SHOT_DURATION_KEY = Util::hash( "END_OF_FRAME_ONE_SHOT_DURATION_KEY" );
} // namespace VTX::App::Internal::Monitoring
#endif
