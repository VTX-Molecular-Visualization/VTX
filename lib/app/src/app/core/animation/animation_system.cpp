#include "app/core/animation/animation_system.hpp"
#include "app/vtx_app.hpp"
#include <util/singleton.hpp>

namespace VTX::App::Core::Animation
{
	AnimationSystem::AnimationSystem()
	{
		_currentAnimationIt = _animationSequence.end();

		// TODO: call from app::update().
		// APP::onUpdate += [ this ]( const float p_deltaTime, const float p_elapsedTime ) { update( p_deltaTime ); };
	}

	void AnimationSystem::play()
	{
		_currentAnimationIt = _animationSequence.begin();
		_isPlaying			= true;

		_playAnimation();
	}
	void AnimationSystem::stop()
	{
		if ( _isPlaying && _currentAnimationIt != _animationSequence.end() )
		{
			( *_currentAnimationIt )->stop();
		}

		_isPlaying			= false;
		_currentAnimationIt = _animationSequence.end();

		onStopped();
	}

	bool AnimationSystem::isPlaying() const { return _isPlaying; }

	void AnimationSystem::update( const float p_deltaTime )
	{
		if ( !_isPlaying )
			return;

		( *_currentAnimationIt )->update( p_deltaTime );
	}

	void AnimationSystem::clear() { _animationSequence.clear(); }

	void AnimationSystem::_playAnimation()
	{
		( *_currentAnimationIt )->onStopped += [ this ]() { _playNextAnimation(); };
		( *_currentAnimationIt )->play();
	}
	void AnimationSystem::_playNextAnimation()
	{
		_currentAnimationIt++;

		if ( _currentAnimationIt == _animationSequence.end() )
		{
			stop();
		}
		else
		{
			_playAnimation();
		}
	}

} // namespace VTX::App::Core::Animation
