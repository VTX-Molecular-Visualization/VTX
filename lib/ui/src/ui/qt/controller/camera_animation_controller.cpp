#include "ui/qt/controller/camera_animation_controller.hpp"
#include "ui/core/animation/animation_system.hpp"
#include "ui/qt/input/input_manager.hpp"

namespace VTX::UI::QT::Controller
{
	void CameraAnimationController::update( const float & p_deltaTime )
	{
		if ( INPUT_MANAGER().isKeyPressed( Input::Key::Key_Escape ) )
		{
			ANIMATION_SYSTEM().stop();
		}
	}
} // namespace VTX::UI::QT::Controller
