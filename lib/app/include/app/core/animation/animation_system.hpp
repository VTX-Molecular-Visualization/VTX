#ifndef __VTX_APP_CORE_ANIMATION_ANIMATION_SYSTEM__
#define __VTX_APP_CORE_ANIMATION_ANIMATION_SYSTEM__

#include "base_animation.hpp"
#include <concepts>
#include <list>
#include <memory>
#include <util/callback.hpp>
#include <util/singleton.hpp>

namespace VTX::App::Core::Animation
{
	template<class T>
	concept AnimationConcept = std::derived_from<T, BaseAnimation>;

	class AnimationSystem
	{
	  public:
		AnimationSystem();
		~AnimationSystem() = default;

		void play();
		void stop();
		bool isPlaying() const;

		void update( const float p_deltaTime );

		template<AnimationConcept A, typename... Args>
		void pushBackAnimation( const Args &... p_args )
		{
			_animationSequence.push_back( std::make_unique<A>( p_args... ) );
		}
		void clear();

		template<AnimationConcept A, typename... Args>
		void launchAnimation( const Args &... p_args )
		{
			stop();
			clear();
			pushBackAnimation<A>( p_args... );
			play();
		}

		Util::Callback<> onStopped;

	  private:
		std::list<std::unique_ptr<BaseAnimation>> _animationSequence = std::list<std::unique_ptr<BaseAnimation>>();
		std::list<std::unique_ptr<BaseAnimation>>::iterator _currentAnimationIt = _animationSequence.end();
		bool												_isPlaying			= false;

		void _playAnimation();
		void _playNextAnimation();
	};

} // namespace VTX::App::Core::Animation

namespace VTX::App
{
	inline Core::Animation::AnimationSystem & ANIMATION_SYSTEM()
	{
		return Util::Singleton<Core::Animation::AnimationSystem>::get();
	}
} // namespace VTX::App

#endif
