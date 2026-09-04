#ifndef __VTX_APP_TRAJECTORY_LOADER__
#define __VTX_APP_TRAJECTORY_LOADER__

#include "app/thread/trajectory_loader.hpp"
#include "app/trajectory/types.hpp"
#include <memory>
#include <utility>

namespace VTX::App::Trajectory
{
	/**
	 * @brief Component that handle loading threaded job.
	 */
	struct Loader
	{
		Loader( std::shared_ptr<Thread::TrajectoryLoader> p_thread, const TRAJECTORY_BUFFER_MODE p_mode ) :
			thread( std::move( p_thread ) ), mode( p_mode ), availableFrames( FrameRange::fromFirstCount( 0, 1 ) )
		{
		}

		std::shared_ptr<Thread::TrajectoryLoader> thread;
		TRAJECTORY_BUFFER_MODE					  mode;
		FrameRange								  availableFrames;
	};
} // namespace VTX::App::Trajectory

#endif
