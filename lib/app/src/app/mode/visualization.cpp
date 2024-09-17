#include "app/mode/visualization.hpp"
#include "app/controller/camera/freefly.hpp"
#include "app/controller/camera/trackball.hpp"
#include "app/core/controller/controller_system.hpp"

namespace VTX::App::Mode
{

	void Visualization::enter() { CONTROLLER_SYSTEM().enableController<Controller::Camera::Trackball>(); }

	void Visualization::exit() {}

	void Visualization::update( const float p_deltaTime, const float p_elapsedTime ) {}
} // namespace VTX::App::Mode
