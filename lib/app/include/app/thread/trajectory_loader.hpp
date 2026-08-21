#ifndef __VTX_APP_THREAD_TRAJECTORY_LOADER__
#define __VTX_APP_THREAD_TRAJECTORY_LOADER__

#include "app/trajectory/types.hpp"
#include <condition_variable>
#include <io/reader.hpp>
#include <mutex>
#include <optional>
#include <util/thread/base_thread.hpp>

namespace VTX::App::Thread
{
	/**
	 * @brief Trajectory loading worker thread.
	 */
	class TrajectoryLoader final : public Util::Thread::BaseThread
	{
		friend class Util::Thread::ThreadManager;

	  public:
		/**
		 * @brief ManagedThread::start().
		 */
		void start();

		/**
		 * @brief BaseThread::stop().
		 */
		void stop() override;

		/**
		 * @brief Update frame request.
		 */
		void requestFrameWindow(
			const Trajectory::FrameRange,
			const Trajectory::TRAJECTORY_READ_DIRECTION,
			const Trajectory::FrameRange
		);

		/**
		 * @brief Cancel request.
		 */
		void cancelFrameRequest();

		/**
		 * @brief Get loaded frames.
		 */
		std::optional<Trajectory::LoadedFrameBatch> takeLoadedFrames();

		/**
		 * @brief Current version.
		 */
		uint64_t getRequestVersion() const;

		/**
		 * @brief Read a specific frame (sync!).
		 */
		Core::Struct::Frame readFrame( const uint );

	  private:
		/**
		 * @brief Constructor.
		 */
		TrajectoryLoader(
			Util::Thread::ThreadManager &,
			IO::SystemReader &&,
			const Trajectory::TRAJECTORY_BUFFER_MODE
		);

		/**
		 * @brief Threaded work.
		 */
		uint _run( Util::Thread::StopToken, Util::Thread::BaseThread & );
		uint _loadFullBuffer( Util::Thread::StopToken, Util::Thread::BaseThread & );
		uint _loadCircularBuffer( Util::Thread::StopToken, Util::Thread::BaseThread & );

		/**
		 * @brief Reader.
		 */
		IO::SystemReader _reader;

		/**
		 * @brief Loading mode.
		 */
		const Trajectory::TRAJECTORY_BUFFER_MODE _mode;

		/**
		 * @brief Mutex.
		 */
		std::mutex				_readerMutex;
		mutable std::mutex		_frameMutex;
		std::condition_variable _frameRequestCondition;

		/**
		 * @brief Current request.
		 */
		std::optional<Trajectory::FrameRange> _requestedFrames;

		/**
		 * @brief Available frames.
		 */
		Trajectory::FrameRange _requestedAvailableFrames;

		/**
		 * @brief Read direction.
		 */
		Trajectory::TRAJECTORY_READ_DIRECTION _requestedDirection = Trajectory::TRAJECTORY_READ_DIRECTION::FORWARD;

		/**
		 * @brief Current version.
		 * Incrementing invalidate previous version.
		 */
		uint64_t _requestVersion = 0;

		/**
		 * @brief Loaded data.
		 */
		std::optional<Trajectory::LoadedFrameBatch> _loadedFrames;
	};
} // namespace VTX::App::Thread

#endif
