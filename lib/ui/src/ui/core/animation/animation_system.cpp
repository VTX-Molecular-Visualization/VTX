#include "ui/core/animation/animation_system.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/controller/base_camera_controller.hpp"
#include "ui/qt/controller/base_picker_controller.hpp"
#include "ui/qt/controller/controller_manager.hpp"
#include "ui/qt/mode/base_mode.hpp"
#include "ui/qt/mode/visualization.hpp"

namespace VTX::UI::Core::Animation
{
	AnimationSystem::AnimationSystem() { _currentAnimationIt = _animationSequence.end(); }

	void AnimationSystem::launchAnimation( const BaseAnimation & p_animation )
	{
		stop();
		clear();
		pushBackAnimation( p_animation );
		play();
	}

	void AnimationSystem::play()
	{
		_currentAnimationIt = _animationSequence.begin();
		_isPlaying			= true;

		App::VTXApp::get().onUpdate().addCallback(
			this, [ this ]( const float p_deltaTime ) { update( p_deltaTime ); }
		);
	}
	void AnimationSystem::stop()
	{
		_isPlaying			= false;
		_currentAnimationIt = _animationSequence.end();

		App::VTXApp::get().onUpdate().removeCallback( this );

		onStopped();
	}

	void AnimationSystem::update( const float p_deltaTime ) { _currentAnimationIt->update( p_deltaTime ); }

	void AnimationSystem::pushBackAnimation( const BaseAnimation & p_animation )
	{
		_animationSequence.push_back( p_animation );
	}
	void AnimationSystem::clear() { _animationSequence.clear(); }

	void AnimationSystem::_playAnimation()
	{
		_currentAnimationIt->onStopped += [ this ]() { _playNextAnimation(); };
		_currentAnimationIt->play();
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

} // namespace VTX::UI::Core::Animation

namespace VTX::UI
{
	UI::Core::Animation::AnimationSystem & ANIMATION_SYSTEM() { return Core::Animation::AnimationSystem::SYSTEM.get(); }
} // namespace VTX::UI
