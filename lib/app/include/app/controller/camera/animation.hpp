#ifndef __VTX_APP_CONTROLLER_CAMERA_ANIMATION__
#define __VTX_APP_CONTROLLER_CAMERA_ANIMATION__

#include "app/core/animation/concepts.hpp"
#include "app/core/controller/base_controller_camera.hpp"
#include "app/core/input/key_mapping.hpp"
#include "app/settings.hpp"
#include <util/hashing.hpp>

namespace VTX::App::Controller::Camera
{
	template<Core::Animation::ConceptAnimation A>
	class Animation : public Core::Controller::BaseControllerCamera
	{
	  public:
		template<typename... Args>
		Animation( Args &&... p_args ) : _animation( std::make_unique<A>( std::forward<Args>( p_args )... ) )
		{
			_animation->play();
		}

		inline void update( const float p_deltaTime, const float p_elapsedTime )
		{
			_animation->update( p_deltaTime, p_elapsedTime );
		}

		inline Util::Callback<> & onAnimationFinished() { return _animation->onEnd; }

	  private:
		std::unique_ptr<A> _animation;
	};
} // namespace VTX::App::Controller::Camera
#endif
