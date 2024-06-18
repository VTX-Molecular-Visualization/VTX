#include "app/core/controller/base_camera_controller.hpp"

namespace VTX::App::Core::Controller
{
	BaseCameraController::BaseCameraController() {}

	void BaseCameraController::setCamera( VTX::App::Component::Render::Camera & p_camera ) { _camera = &p_camera; }

	void BaseCameraController::update( const float & p_deltaTime )
	{
		if ( isActive() )
			_updateInputs( p_deltaTime );
	}
} // namespace VTX::App::Core::Controller
