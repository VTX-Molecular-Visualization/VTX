// #include "app/core/trajectory_player/base_player.hpp"
// #include <core/struct/trajectory.hpp>
//
// namespace VTX::App::Core::TrajectoryPlayer
//{
//	void BasePlayer::setTrajectory( VTX::Core::Struct::Trajectory & p_trajectory ) { _trajectoryPtr = &p_trajectory; }
//
//	size_t BasePlayer::getCurrentFrameIndex() const { return _trajectoryPtr->currentFrameIndex; }
//	void   BasePlayer::setCurrentFrameIndex( const size_t p_frameIndex )
//	{
//		_trajectoryPtr->currentFrameIndex = p_frameIndex;
//	}
//	size_t BasePlayer::getFrameCount() const { return _trajectoryPtr->frames.size(); }
//
//	void BasePlayer::play() { _isPlaying = true; }
//	void BasePlayer::pause() { _isPlaying = false; }
//	void BasePlayer::stop()
//	{
//		pause();
//		reset();
//	}
//
//	void BasePlayer::update( const float p_deltaTime )
//	{
//		if ( !isPlaying() )
//			return;
//
//		_trajectoryTimer += p_deltaTime;
//
//		if ( _fps == 0u )
//		{
//			nextFrame();
//		}
//		else
//		{
//			const size_t currentFrameIndex = getCurrentFrameIndex();
//			size_t		 nextFrameIndex	   = currentFrameIndex;
//
//			const float offset = 1.f / float( _fps );
//			while ( _trajectoryTimer >= offset )
//			{
//				nextFrameIndex++;
//				_trajectoryTimer -= offset;
//			}
//
//			if ( nextFrameIndex != currentFrameIndex )
//			{
//				nextFrame( nextFrameIndex - currentFrameIndex );
//			}
//		}
//	}
//
//	void BasePlayer::setFPS( const uint p_fps ) { _fps = p_fps; }
// } // namespace VTX::App::Core::TrajectoryPlayer
