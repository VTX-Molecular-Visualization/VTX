#ifndef __VTX_UI_CORE_ANIMATION_ANIMATION_SYSTEM__
#define __VTX_UI_CORE_ANIMATION_ANIMATION_SYSTEM__

#include "ui/core/animation/base_animation.hpp"
#include <app/application/system/system_registration.hpp>
#include <app/core/system/base_system.hpp>
#include <list>
#include <util/callback.hpp>

namespace VTX::UI::Core::Animation
{
	class AnimationSystem : public App::Core::System::BaseSystem
	{
	  public:
		inline static const App::Application::System::SystemRegistration<AnimationSystem> SYSTEM
			= App::Application::System::SystemRegistration<AnimationSystem>();

	  public:
		AnimationSystem();
		~AnimationSystem() = default;

		void launchAnimation( const BaseAnimation & p_animation );

		void play();
		void stop();

		void update( const float p_deltaTime );

		void pushBackAnimation( const BaseAnimation & p_animation );
		void clear();

		Util::Callback<> onStopped;

	  private:
		std::list<BaseAnimation>		   _animationSequence  = std::list<BaseAnimation>();
		std::list<BaseAnimation>::iterator _currentAnimationIt = _animationSequence.end();
		bool							   _isPlaying		   = false;

		void _playAnimation();
		void _playNextAnimation();
	};
} // namespace VTX::UI::Core::Animation

namespace VTX::UI
{
	UI::Core::Animation::AnimationSystem & ANIMATION_SYSTEM();
}

#endif
