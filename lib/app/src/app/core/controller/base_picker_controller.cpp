#include "app/core/controller/base_picker_controller.hpp"

namespace VTX::App::Core::Controller
{
	BasePickerController::BasePickerController() {}

	void BasePickerController::setCamera( VTX::App::Component::Render::Camera & p_camera ) { _camera = &p_camera; }

	void BasePickerController::update( const float & p_deltaTime ) {}
} // namespace VTX::App::Core::Controller
