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
			explicit Snapshot( const Worker::Snapshoter::MODE p_mode, const Path & p_path ) :
				_mode( p_mode ), _path( p_path )
			{
			}

			virtual void execute() override
			{
				Worker::Snapshoter snapshoter;

				if ( _mode == Worker::Snapshoter::MODE::GL && snapshoter.takeSnapshotGL( _path ) )
				{
					VTX_INFO( "Snapshot taken: " + _path.filename().string() );
				}
				else if ( _mode == Worker::Snapshoter::MODE::RT && snapshoter.takeSnapshotRT( _path ) )
				{
					VTX_INFO( "Render computed: " + _path.filename().string() );
				}
				else
				{
					VTX_WARNING( "Failed: " + _path.string() );
				}
			};

			virtual void displayUsage() override { VTX_INFO( "No parameters" ); }

		  private:
			const Worker::Snapshoter::MODE _mode;
			const Path					   _path;
		};
	} // namespace Action
} // namespace VTX
#endif
