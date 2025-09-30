#ifndef __VTX_APP_PIPELINE__
#define __VTX_APP_PIPELINE__

#include <util/hashing.hpp>

namespace VTX::Util::Monitoring
{
	class Stats;
}

namespace VTX::App
{
	/**
	 * @brief Defines the main application pipeline steps and process them.
	 */
	struct Pipeline
	{
		inline static const Hash UPDATE		 = Util::hash( "UPDATE" );
		inline static const Hash POST_UPDATE = Util::hash( "POST_UPDATE" );
		inline static const Hash RENDER		 = Util::hash( "RENDER" );
		inline static const Hash POST_RENDER = Util::hash( "POST_RENDER" );

		/**
		 * @brief Process each step and log their duration.
		 */
		void process( Util::Monitoring::Stats &, const float p_delta, const float p_elapsed );
	};

} // namespace VTX::App

#endif
