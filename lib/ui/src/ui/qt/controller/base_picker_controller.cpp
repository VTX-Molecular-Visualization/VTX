#include "ui/qt/controller/base_picker_controller.hpp"

namespace VTX::UI::QT::Controller
{
	BasePickerController::BasePickerController() {}

	void BasePickerController::setCamera( VTX::App::Component::Render::Camera & p_camera ) { _camera = &p_camera; }

	void BasePickerController::update( const float & p_deltaTime ) {}
} // namespace VTX::UI::QT::Controller
