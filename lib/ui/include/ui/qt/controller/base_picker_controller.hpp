#ifndef __VTX_UI_QT_CONTROLLER_BASE_PICKER_CONTROLLER__
#define __VTX_UI_QT_CONTROLLER_BASE_PICKER_CONTROLLER__

#include "ui/qt/controller/base_controller.hpp"
#include <QWidget>
#include <app/component/render/camera.hpp>
#include <util/types.hpp>

namespace VTX::UI::QT::Controller
{
	class BasePickerController : public BaseController
	{
	  public:
		explicit BasePickerController( VTX::App::Component::Render::Camera & p_camera );
		virtual ~BasePickerController() = default;

		inline bool isActive() const { return _active; }
		inline void setActive( const bool p_active ) { _active = p_active; }

		virtual void update( const float & p_deltaTime ) override;
		virtual void reset() {}

	  protected:
		bool							 _active = true;
		App::Component::Render::Camera & _camera;

		virtual void _updateInputs( const float & ) = 0;
	};
} // namespace VTX::UI::QT::Controller
#endif
