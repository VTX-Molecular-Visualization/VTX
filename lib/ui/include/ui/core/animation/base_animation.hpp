#ifndef __VTX_UI_CORE_ANIMATION_BASE_ANIMATION__
#define __VTX_UI_CORE_ANIMATION_BASE_ANIMATION__

#include <app/core/callback_event.hpp>

namespace VTX::UI::Core::Animation
{
	struct BaseAnimationInfo
	{
		BaseAnimationInfo() = default;
		BaseAnimationInfo( const float p_duration ) : duration( p_duration ) {}

		float duration = 0.f;
	};

	class BaseAnimation
	{
	  public:
		BaseAnimation() = default;
		BaseAnimation( const BaseAnimation & p_source );
		BaseAnimation( const BaseAnimationInfo & p_info );
		virtual ~BaseAnimation() = default;

		void update( float p_deltaTime );

		void		 play();
		virtual bool canBeStopped() { return true; }
		void		 stop();

		bool  isRunning() { return _isRunning; }
		float getRatio() const;

		App::Core::CallbackEmitter<> onStopped = App::Core::CallbackEmitter<>();

	  protected:
		virtual void _enter() {}
		virtual void _internalUpdate( float p_deltaTime ) = 0;
		virtual void _exit() {}

		BaseAnimationInfo _info = BaseAnimationInfo();

		float _currentTime = 0.f;
		bool  _isRunning   = false;
	};
} // namespace VTX::UI::Core::Animation

#endif
