#ifndef __VTX_ACTION_SNAPSHOT__
#define __VTX_ACTION_SNAPSHOT__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "util./filesystem.hpp"
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
			Snapshot( const Worker::Snapshoter::MODE p_mode ) : _mode( p_mode ) {}
			Snapshot( const Worker::Snapshoter::MODE p_mode, const std::string & p_fileName ) :
				_mode( p_mode ), _fileName( p_fileName )
			{
			}

			virtual void setParameters( const std::vector<std::string> & p_parameters ) override {}

			virtual void execute() override
			{
				Worker::Snapshoter snapshoter;

				const IO::Path & path = _mode == Worker::Snapshoter::MODE::GL
											? Util::Filesystem::getSnapshotsPath( _fileName + ".png" )
											: Util::Filesystem::getRendersPath( _fileName + ".png" );

				if ( _mode == Worker::Snapshoter::MODE::GL && snapshoter.takeSnapshotGL( path ) )
				{
					VTX_INFO( "Snapshot taken: " + path.getFileName() );
				}
				else if ( _mode == Worker::Snapshoter::MODE::RT && snapshoter.takeSnapshotRT( path ) )
				{
					VTX_INFO( "Render computed: " + path.getFileName() );
				}
				else
				{
					VTX_WARNING( "Snapshot failed" );
				}
			};

			virtual void displayUsage() override { VTX_INFO( "No parameters" ); }

		  private:
			const Worker::Snapshoter::MODE _mode;
			const std::string			   _fileName = Util::Time::getTimestamp();
		};
	} // namespace Action
} // namespace VTX
#endif
