#ifndef __VTX_APP_CORE_CONTROLLER_BASE_CONTROLLER_CAMERA__
#define __VTX_APP_CORE_CONTROLLER_BASE_CONTROLLER_CAMERA__

#include "app/component/render/camera.hpp"
#include "app/core/input/key_mapping.hpp"
#include "base_controller.hpp"

namespace VTX::App::Core::Controller
{

	class BaseControllerCamera : public BaseController
	{
	  public:
		virtual ~BaseControllerCamera() = default;

		void setCamera( Component::Render::Camera * const p_camera ) { _camera = p_camera; }

	  protected:
		Component::Render::Camera * _camera;
		Core::Input::KeyMapping		_mapping;
	};

	/*
	template<Animation::ConceptAnimation A>
	class ControllerCameraAnimation
	{
	  public:
		// TODO
	  private:
		A _animation;
	};
	*/

} // namespace VTX::App::Core::Controller

#endif
