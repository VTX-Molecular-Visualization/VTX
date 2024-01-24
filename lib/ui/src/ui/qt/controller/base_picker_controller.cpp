#include "ui/qt/controller/base_picker_controller.hpp"

namespace VTX::UI::QT::Controller
{
	BasePickerController::BasePickerController( VTX::App::Component::Render::Camera & p_camera ) : _camera( p_camera )
	{
	}

	void BasePickerController::update( const float & p_deltaTime ) { _updateInputs( p_deltaTime ); }
} // namespace VTX::UI::QT::Controller
