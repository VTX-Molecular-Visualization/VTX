#ifndef __VTX_APP_CORE_TRAJECTORY_PLAYER_BASE_PLAYER__
#define __VTX_APP_CORE_TRAJECTORY_PLAYER_BASE_PLAYER__

#include "app/core/collection.hpp"
#include "app/core/collectionable.hpp"
#include <core/struct/trajectory.hpp>
#include <memory>
#include <string>
#include <util/types.hpp>

namespace VTX::App::Core::TrajectoryPlayer
{
	class BasePlayer : public App::Core::DisplayableCollectionable
	{
	  public:
		BasePlayer()							  = default;
		BasePlayer( const BasePlayer & p_source ) = default;
		BasePlayer( VTX::Core::Struct::Trajectory * const p_trajectory ) : _trajectoryPtr( p_trajectory ) {}

		virtual ~BasePlayer() = default;

		bool isLinkedToTrajectory() const { return _trajectoryPtr != nullptr; }
		void setTrajectory( VTX::Core::Struct::Trajectory & p_trajectory );

		size_t getCurrentFrameIndex() const;
		void   setCurrentFrameIndex( const size_t p_frameIndex );
		size_t getFrameCount() const;

		virtual void play();
		virtual void pause();
		virtual void stop();

		virtual void update( const float p_deltaTime );

		inline bool isPlaying() const { return _isPlaying; }

		virtual void reset()									= 0;
		virtual void nextFrame( const size_t p_frameCount = 1 ) = 0;

		inline uint getFPS() const { return _fps; }
		void		setFPS( const uint p_fps );

		virtual const CollectionKey &		getCollectionKey() const = 0;
		virtual std::unique_ptr<BasePlayer> clone() const			 = 0;

	  private:
		VTX::Core::Struct::Trajectory * _trajectoryPtr = nullptr;

		bool  _isPlaying	   = true;
		uint  _fps			   = 1u;
		float _trajectoryTimer = 0;
	};
} // namespace VTX::App::Core::TrajectoryPlayer
#endif
