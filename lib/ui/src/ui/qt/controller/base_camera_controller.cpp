#include "ui/qt/controller/base_camera_controller.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/widget/renderer/panel.hpp"

namespace VTX::UI::QT::Controller
{
	BaseCameraController::BaseCameraController() {}

	void BaseCameraController::setCamera( VTX::App::Component::Render::Camera & p_camera ) { _camera = &p_camera; }

	void BaseCameraController::update( const float & p_deltaTime )
	{
		if ( isActive() )
			_updateInputs( p_deltaTime );
	}
} // namespace VTX::UI::QT::Controller
