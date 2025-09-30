#ifndef __VTX_APP_PIPELINE__
#define __VTX_APP_PIPELINE__

#include "app/ecs.hpp"
#include "app/events.hpp"
#include <util/hashing.hpp>
#include <util/logger.hpp>
#include <util/monitoring/stats.hpp>

namespace VTX::App
{

	struct Pipeline
	{
		inline static const Hash TICK_RATE	 = Util::hash( "TICK_RATE" );
		inline static const Hash POST_UPDATE = Util::hash( "POST_UPDATE" );
		inline static const Hash UPDATE		 = Util::hash( "UPDATE" );
		inline static const Hash RENDER		 = Util::hash( "RENDER" );
		inline static const Hash POST_RENDER = Util::hash( "POST_RENDER" );

		void process( Util::Monitoring::Stats &, const float p_delta, const float p_elapsed );
	};

} // namespace VTX::App

#endif
