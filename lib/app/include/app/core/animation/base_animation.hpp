#ifndef __VTX_APP_CORE_ANIMATION_BASE_ANIMATION__
#define __VTX_APP_CORE_ANIMATION_BASE_ANIMATION__

#include <util/callback.hpp>

namespace VTX::App::Core::Animation
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

		Util::Callback<> onStopped;

	  protected:
		virtual void _enter() {}
		virtual void _internalUpdate( float p_deltaTime ) = 0;
		virtual void _exit() {}

		BaseAnimationInfo _info;

		float _currentTime = 0.f;
		bool  _isRunning   = false;
	};
} // namespace VTX::App::Core::Animation

#endif
