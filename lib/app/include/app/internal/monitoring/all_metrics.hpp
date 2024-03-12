#ifndef __VTX_APP_INTERNAL_MONITORING_ALL_METRICS__
#define __VTX_APP_INTERNAL_MONITORING_ALL_METRICS__

#include "app/core/monitoring/frame_info.hpp"
#include <util/hashing.hpp>

namespace VTX::App::Internal::Monitoring
{
	const size_t TICK_RATE_KEY		 = Util::Hashing::hash( "TICK_RATE_KEY" );
	const size_t DRAW_CALL_COUNT_KEY = Util::Hashing::hash( "DRAW_CALL_COUNT" );

	const size_t PRE_UPDATE_DURATION_KEY  = Util::Hashing::hash( "PRE_UPDATE_DURATION" );
	const size_t UPDATE_DURATION_KEY	  = Util::Hashing::hash( "UPDATE_DURATION" );
	const size_t LATE_UPDATE_DURATION_KEY = Util::Hashing::hash( "LATE_UPDATE_DURATION" );
	const size_t POST_UPDATE_DURATION_KEY = Util::Hashing::hash( "POST_UPDATE_DURATION" );

	const size_t PRE_RENDER_DURATION_KEY  = Util::Hashing::hash( "PRE_RENDER_DURATION" );
	const size_t RENDER_DURATION_KEY	  = Util::Hashing::hash( "RENDER_DURATION" );
	const size_t POST_RENDER_DURATION_KEY = Util::Hashing::hash( "POST_RENDER_DURATION" );

	const size_t END_OF_FRAME_ONE_SHOT_DURATION_KEY = Util::Hashing::hash( "END_OF_FRAME_ONE_SHOT_DURATION_KEY" );
} // namespace VTX::App::Internal::Monitoring
#endif
