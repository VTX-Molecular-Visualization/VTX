#ifndef __VTX_APP_EXTRACTOR_TRAJECTORY_BUFFER__
#define __VTX_APP_EXTRACTOR_TRAJECTORY_BUFFER__

#include "app/trajectory/types.hpp"
#include <cstddef>

namespace VTX::App::Extractor
{
	/**
	 * @brief Trajectory buffer allocation settings.
	 */
	struct TrajectoryBufferSettings
	{
		// Default threshold to switch between strategies.
		static constexpr size_t DEFAULT_MAX_FULL_BUFFER_BYTE_SIZE = size_t( 1024 ) * 1024 * 1024;
		// Window default size.
		static constexpr size_t DEFAULT_CIRCULAR_BUFFER_FRAME_COUNT = 20;

		// Threshold.
		size_t maxFullBufferByteSize = DEFAULT_MAX_FULL_BUFFER_BYTE_SIZE;
		// Window size.
		size_t circularBufferFrameCount = DEFAULT_CIRCULAR_BUFFER_FRAME_COUNT;

		Trajectory::TRAJECTORY_BUFFER_MODE selectMode( const size_t, const size_t ) const noexcept;
		size_t getStorageFrameCount( const Trajectory::TRAJECTORY_BUFFER_MODE, const size_t ) const noexcept;
	};
} // namespace VTX::App::Extractor

#endif
