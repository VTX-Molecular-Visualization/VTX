#include "app/extractor/trajectory_buffer.hpp"
#include <algorithm>
#include <util/types.hpp>

namespace VTX::App::Extractor
{
	Trajectory::TRAJECTORY_BUFFER_MODE TrajectoryBufferSettings::selectMode(
		const size_t p_atomCount,
		const size_t p_frameCount
	) const noexcept
	{
		if ( p_atomCount == 0 || p_frameCount == 0 )
		{
			return Trajectory::TRAJECTORY_BUFFER_MODE::FULL;
		}

		const size_t maxFullBufferFrameCount = maxFullBufferByteSize / sizeof( Vec3f ) / p_atomCount;
		return p_frameCount > maxFullBufferFrameCount ? Trajectory::TRAJECTORY_BUFFER_MODE::CIRCULAR
													  : Trajectory::TRAJECTORY_BUFFER_MODE::FULL;
	}

	size_t TrajectoryBufferSettings::getStorageFrameCount(
		const Trajectory::TRAJECTORY_BUFFER_MODE p_mode,
		const size_t							 p_frameCount
	) const noexcept
	{
		return p_mode == Trajectory::TRAJECTORY_BUFFER_MODE::CIRCULAR
				   ? std::min( p_frameCount, std::max( size_t( 1 ), circularBufferFrameCount ) )
				   : p_frameCount;
	}
} // namespace VTX::App::Extractor
