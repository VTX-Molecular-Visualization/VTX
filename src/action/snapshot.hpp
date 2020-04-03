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
			Snapshot() {}
			Snapshot( std::string & p_fileName ) : _fileName( p_fileName ) {}

			virtual void setParameters( std::vector<std::string> & p_parameters ) override {}

			virtual void execute() override
			{
				Worker::Snapshoter snapshoter;

				IO::Path path( _fileName + ".png" );
				if ( snapshoter.takeSnapshot( path ) ) { VTX_INFO( "Snapshot taken: " + path.getFileName() ); }
				else
				{
					VTX_WARNING( "Snapshot failed" );
				}
			};

			virtual void displayUsage() override { VTX_INFO( "No parameters" ); }

		  private:
			std::string _fileName = SNAPSHOT_DIR + Util::Time::getTimestamp();
		};
	} // namespace Action
} // namespace VTX
#endif
