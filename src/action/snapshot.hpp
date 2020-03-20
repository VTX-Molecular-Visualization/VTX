#ifndef __VTX_ACTION_SNAPSHOT__
#define __VTX_ACTION_SNAPSHOT__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "util/logger.hpp"
#include "util/time.hpp"
#include "worker/snapshoter.hpp"

namespace VTX
{
	namespace Action
	{
		class Snapshot : public BaseAction
		{
		  public:
			virtual void execute() override
			{
				Worker::Snapshoter snapshoter;

				std::string filename = Util::Time::getTimestamp();
				IO::Path	path( SNAPSHOT_DIR + filename + ".png" );

				if ( snapshoter.takeSnapshot( path ) ) { VTX_INFO( "Snapshot taken: " + path.getFileName() ); }
				else
				{
					VTX_WARNING( "Snapshot failed" );
				}
			};
		};
	} // namespace Action
} // namespace VTX
#endif
