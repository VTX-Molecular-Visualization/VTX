#ifndef __VTX_APP_CORE_TRAJECTORY_PLAYER_BASE_PLAYER__
#define __VTX_APP_CORE_TRAJECTORY_PLAYER_BASE_PLAYER__

#include <concepts>
#include <core/struct/trajectory.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::TrajectoryPlayer
{
	// template<typename T>
	// concept TrajectoryPlayMode = requires { true; };

	// template<TrajectoryPlayMode PlayMode>
	class BasePlayer
	{
	  public:
		BasePlayer()							  = default;
		BasePlayer( const BasePlayer & p_source ) = default;
		BasePlayer( VTX::Core::Struct::Trajectory * const p_trajectory ) : _trajectoryPtr( p_trajectory ) {}

		virtual ~BasePlayer() = default;

		void setTrajectory( VTX::Core::Struct::Trajectory & p_trajectory );

		size_t getCurrentFrameIndex() const;
		void   setCurrentFrameIndex( const size_t p_frameIndex );
		size_t getFrameCount() const;

		void play();
		void pause();
		void stop();

		void update( const float p_deltaTime );

		inline bool isPlaying() const { return _isPlaying; }

		virtual void reset()									= 0;
		virtual void nextFrame( const size_t p_frameCount = 1 ) = 0;

		inline uint getFPS() const { return _fps; }
		void		setFPS( const uint p_fps );

	  protected:
		std::string _displayName;

	  private:
		VTX::Core::Struct::Trajectory * _trajectoryPtr = nullptr;
		// std::unique_ptr<TrajectoryPlayMode> _playmode;

		size_t _currentFrame	 = 0;
		bool   _isPlaying		 = true;
		uint   _fps				 = 1u;
		uint   _dynamicLoopCount = 0;
		float  _trajectoryTimer	 = 0;
	};
} // namespace VTX::App::Core::TrajectoryPlayer
#endif
