#include "app/core/player/base_player.hpp"
#include <app/component/chemistry/trajectory.hpp>
#include <app/component/render/proxy_system.hpp>
#include <app/core/ecs/registry.hpp>
#include <util/math.hpp>

namespace VTX::App::Core::Player
{
	void BasePlayer::setCount( const size_t p_count )
	{
		_count = p_count;

		if ( _current >= p_count )
		{
			_current = p_count - 1;
			for ( auto iter = App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Trajectory>().begin();
				  iter != App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Trajectory>().end();
				  ++iter )
			{
				auto & trajectory = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( *iter );

				if ( &( trajectory.getPlayer() ) == this )
				{
					auto   entity	= App::ECS_REGISTRY().getEntity( trajectory );
					auto & molecule = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::System>( entity );
					VTX::App::Component::Render::ProxySystem & proxy
						= App::ECS_REGISTRY().getComponent<App::Component::Render::ProxySystem>( entity );

					molecule.getTrajectory().setCurrentFrameIndex( _current );
					onFrameChange( molecule.getTrajectory().getCurrentFrame() );
				}
			}
		}
	}

	size_t BasePlayer::getCurrent() const { return _current; }
	void   BasePlayer::setCurrent( const size_t p_frameIndex )
	{
		if ( _current != p_frameIndex )
		{
			_current = p_frameIndex;
			for ( auto iter = App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Trajectory>().begin();
				  iter != App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Trajectory>().end();
				  ++iter )
			{
				auto & trajectory = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( *iter );

				if ( &( trajectory.getPlayer() ) == this )
				{
					auto   entity	= App::ECS_REGISTRY().getEntity( trajectory );
					auto & molecule = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::System>( entity );
					VTX::App::Component::Render::ProxySystem & proxy
						= App::ECS_REGISTRY().getComponent<App::Component::Render::ProxySystem>( entity );

					molecule.getTrajectory().setCurrentFrameIndex( p_frameIndex );
					onFrameChange( molecule.getTrajectory().getCurrentFrame() );
				}
			}
		}
	}

	void BasePlayer::play()
	{
		_isPlaying = true;
		onPlay();
	}
	void BasePlayer::pause()
	{
		_isPlaying = false;
		onPause();
	}
	void BasePlayer::stop()
	{
		_isPlaying = false;
		reset();
		onStop();
	}

	void BasePlayer::update( const float p_deltaTime, const float p_elapsedTime )
	{
		if ( not isPlaying() )
		{
			return;
		}

		/* _trajectoryTimer += p_deltaTime;

		if ( _fps == 0u )
		{
			nextFrame();
		}
		else
		{
			const size_t previousCurrent = _current;
			size_t		 nextIndex		 = _current;

			const float offset = 1.f / float( _fps );
			while ( _trajectoryTimer >= offset )
			{
				nextIndex++;
				_trajectoryTimer -= offset;
			}

			if ( nextIndex != previousCurrent )
			{
				nextFrame( nextIndex - previousCurrent );
			}
		}*/

		//////////
		if ( _fps == 0u )
		{
			nextFrame( 1 );
		}
		else
		{
			const float frameRateMilliSec = ( 1.f / float( _fps ) ) * 1000.f;
			const float ellapsedTime	  = p_elapsedTime - _trajectoryTimer;

			if ( ellapsedTime >= frameRateMilliSec )
			{
				_trajectoryTimer = p_elapsedTime;
				nextFrame( 1 );
			}
		}
		//////////
	}

	void BasePlayer::setFPS( const uint p_fps )
	{
		if ( _fps != p_fps )
		{
			_fps = p_fps;
			onFPSChange( p_fps );
		}
	}
} // namespace VTX::App::Core::Player
