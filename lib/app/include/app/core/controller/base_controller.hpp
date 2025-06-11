#ifndef __VTX_APP_CORE_CONTROLLER_BASE_CONTROLLER__
#define __VTX_APP_CORE_CONTROLLER_BASE_CONTROLLER__

#include "app/component/render/camera.hpp"
#include "app/core/input/key_mapping.hpp"
namespace VTX::App::Core::Controller
{

	class BaseController
	{
	  public:
		virtual ~BaseController() = default;
		void setCamera( Component::Render::Camera * const p_camera ) { _camera = p_camera; }

		virtual bool isActive() { return true; }

	  protected:
		Component::Render::Camera * _camera;
		Core::Input::KeyMapping		_mapping;
	};

} // namespace VTX::App::Core::Controller

#endif
