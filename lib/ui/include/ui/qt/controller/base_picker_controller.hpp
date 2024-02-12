#ifndef __VTX_UI_QT_CONTROLLER_BASE_PICKER_CONTROLLER__
#define __VTX_UI_QT_CONTROLLER_BASE_PICKER_CONTROLLER__

#include "ui/qt/controller/base_controller.hpp"
#include <app/component/render/camera.hpp>

namespace VTX::UI::QT::Controller
{
	class BasePickerController : public BaseController
	{
	  public:
		BasePickerController();
		virtual ~BasePickerController() = default;

		void setCamera( VTX::App::Component::Render::Camera & p_camera );

		inline bool isActive() const { return _active; }
		inline void setActive( const bool p_active ) { _active = p_active; }

		virtual void update( const float & p_deltaTime ) override;
		virtual void reset() {}

	  protected:
		const App::Component::Render::Camera & getCamera() const { return *_camera; }

	  private:
		bool							 _active = true;
		App::Component::Render::Camera * _camera = nullptr;
	};
} // namespace VTX::UI::QT::Controller
#endif
