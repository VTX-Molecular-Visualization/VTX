#ifndef __VTX_APP_CORE_CONTROLLER_BASE_CONTROLLER__
#define __VTX_APP_CORE_CONTROLLER_BASE_CONTROLLER__

#include "app/component/render/camera.hpp"
#include "app/input/key_mapping.hpp"

namespace VTX::App::Controller
{

	class BaseController
	{
	  public:
		virtual ~BaseController() = default;
		void setCamera( Component::Render::Camera * const p_camera ) { _camera = p_camera; }

		virtual bool isActive() { return true; }

	  protected:
		Component::Render::Camera * _camera;
		Input::KeyMapping			_mapping;
	};

} // namespace VTX::App::Controller

#endif
