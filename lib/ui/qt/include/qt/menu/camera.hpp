#ifndef __VTX_UI_QT_MENU_CAMERA__
#define __VTX_UI_QT_MENU_CAMERA__

#include "qt/base_widget.hpp"
#include <QMenu>

namespace VTX::UI::QT::Menu
{

	class Camera : public BaseWidget<Camera, QMenu>
	{
	  public:
		Camera( QWidget * p_parent ) : BaseWidget<Camera, QMenu>( "Camera", p_parent )
		{
			QMenu * projectionMenu = addMenu( "Projection" );
			projectionMenu->addAction( ACTION<UI::Action::Camera::Perspective>() );
			projectionMenu->addAction( ACTION<UI::Action::Camera::Orthographic>() );

			QMenu * controllerMenu = addMenu( "Controller" );
			controllerMenu->addAction( ACTION<UI::Action::Camera::Trackball>() );
			controllerMenu->addAction( ACTION<UI::Action::Camera::Freefly>() );

			addAction<UI::Action::Camera::Orient>();
			addAction<UI::Action::Camera::Reset>();
		}
		virtual ~Camera() {}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
