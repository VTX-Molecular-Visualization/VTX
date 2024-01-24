#ifndef __VTX_UI_QT_CONTROLLER_BASE_CAMERA_CONTROLLER__
#define __VTX_UI_QT_CONTROLLER_BASE_CAMERA_CONTROLLER__

#include "ui/qt/controller/base_controller.hpp"
#include <QWidget>
#include <app/component/render/camera.hpp>
#include <util/types.hpp>

namespace VTX::UI::QT::Controller
{
	class BaseCameraController : public BaseController
	{
	  public:
		explicit BaseCameraController();
		virtual ~BaseCameraController() = default;

		void setCamera( VTX::App::Component::Render::Camera & p_camera );

		inline bool isActive() const { return _active; }
		inline void setActive( const bool p_active ) { _active = p_active; }

		virtual void update( const float & p_deltaTime ) override;
		virtual void reset() {}

		virtual bool canBeStopped() { return true; };

		inline void setWidgetTarget( const QWidget * const p_widget ) { _widgetTarget = p_widget; }

	  protected:
		App::Component::Render::Camera & getCamera() { return *_camera; }

		const QWidget * _widgetTarget = nullptr;

	  private:
		bool							 _active = true;
		App::Component::Render::Camera * _camera = nullptr;

		virtual void _updateInputs( const float & ) = 0;
	};
} // namespace VTX::UI::QT::Controller
#endif
