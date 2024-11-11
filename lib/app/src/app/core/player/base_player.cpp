#include "app/core/player/base_player.hpp"
#include <util/math.hpp>
//devjla
#include <app/component/render/proxy_molecule.hpp>
#include <app/core/ecs/registry.hpp>
#include <app/component/chemistry/trajectory.hpp>

namespace VTX::App::Core::Player
{
	void BasePlayer::setCount( const size_t p_count )
	{
		_count = p_count;

		if ( _current >= p_count )
		{
			_current = p_count - 1;
			//onFrameChange( _current );
			for ( auto iter = App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Trajectory>().begin();
				  iter != App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Trajectory>().end();
				  ++iter )
			{
				auto & trajectory = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( *iter );

				if ( &( trajectory.getPlayer() ) == this )
				{
					auto   entity	= App::ECS_REGISTRY().getEntity( trajectory );
					auto & molecule = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Molecule>( entity );
					VTX::App::Component::Render::ProxyMolecule & proxy
						= App::ECS_REGISTRY().getComponent<App::Component::Render::ProxyMolecule>( entity );

					molecule.getTrajectory()._currentFrameIndex = _current;
					onFrameChange( molecule.getTrajectory()._framesVector[ _current ] );
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
			//onFrameChange( p_frameIndex );
			for ( auto iter = App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Trajectory>().begin();
				  iter != App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Trajectory>().end();
				  ++iter )
			{
				auto & trajectory = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( *iter );

				if ( &( trajectory.getPlayer() ) == this )
				{
					auto   entity	= App::ECS_REGISTRY().getEntity( trajectory );
					auto & molecule = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Molecule>( entity );
					VTX::App::Component::Render::ProxyMolecule & proxy
						= App::ECS_REGISTRY().getComponent<App::Component::Render::ProxyMolecule>( entity );

					molecule.getTrajectory()._currentFrameIndex = p_frameIndex;
					onFrameChange( molecule.getTrajectory()._framesVector[ p_frameIndex ] );
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

	void BasePlayer::update( const float p_deltaTime )
	{
		if ( !isPlaying() )
			return;

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
			nextFrame();
		}
		else
		{
			const float frameRateMilliSec = ( 1.f / float( _fps ) ) * 1000.f;
			const float ellapsedTime	  = p_deltaTime - _trajectoryTimer;
			if (ellapsedTime >= frameRateMilliSec)
			{
				_trajectoryTimer = p_deltaTime;
				nextFrame( _current + 1);
			}
		}
		//////////


		// devjla
		/* VTX::Core::Struct::Frame currentFrame;
		if ( !_tmpFrames.GetCopyFrame( currentFrame ) )
			return;
		VTX::App::Component::Render::ProxyMolecule & proxy
			= ECS_REGISTRY().getComponent<App::Component::Render::ProxyMolecule>( *(ECS_REGISTRY().findComponents<App::Component::Render::ProxyMolecule>().begin()));
		proxy._updateAtomsPositions( currentFrame );*/
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
