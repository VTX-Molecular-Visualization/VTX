#include "app/core/player/base_player.hpp"
#include <util/math.hpp>
//devjla
#include "app/component/render/proxy_molecule.hpp"

namespace VTX::App::Core::Player
{
	void BasePlayer::setCount( const size_t p_count )
	{
		_count = p_count;

		if ( _current >= p_count )
		{
			_current = p_count - 1;
			onFrameChange( _current );
		}
	}

	size_t BasePlayer::getCurrent() const { return _current; }
	void   BasePlayer::setCurrent( const size_t p_frameIndex )
	{
		if ( _current != p_frameIndex )
		{
			_current = p_frameIndex;
			onFrameChange( p_frameIndex );
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

		_trajectoryTimer += p_deltaTime;

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
		}

		// devjla
		VTX::Core::Struct::Frame currentFrame;
		if ( !_tmpFrames.GetCopyFrame( currentFrame ) )
			return;
		VTX::App::Component::Render::ProxyMolecule & proxy
			= App::MAIN_REGISTRY().findComponent<App::Component::Render::ProxyMolecule>();
		proxy._updateAtomsPositions( currentFrame );
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
