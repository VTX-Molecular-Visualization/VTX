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
			projectionMenu->addAction( ACTION<Action::Camera::Perspective>() );
			projectionMenu->addAction( ACTION<Action::Camera::Orthographic>() );

			QMenu * controllerMenu = addMenu( "Controller" );
			controllerMenu->addAction( ACTION<Action::Camera::Trackball>() );
			controllerMenu->addAction( ACTION<Action::Camera::Freefly>() );

			addAction<Action::Camera::Orient>();
			addAction<Action::Camera::Reset>();
		}
		virtual ~Camera() {}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
