#ifndef __VTX_UI_QT_MENU_CAMERA__
#define __VTX_UI_QT_MENU_CAMERA__

#include "ui/qt/base_widget.hpp"
#include <QMenu>
#include <app/controller/camera/freefly.hpp>
#include <app/controller/camera/trackball.hpp>
#include <app/core/controller/controller_system.hpp>

namespace VTX::UI::QT::Menu
{
	class Camera : public BaseWidget<Camera, QMenu>
	{
	  public:
		Camera( QWidget * p_parent ) : BaseWidget<Camera, QMenu>( "Camera", p_parent )
		{
			addAction<Action::Camera::Perspective>();
			addAction<Action::Camera::Orthographic>();
			addSeparator();
			QAction * const trackball = addAction<Action::Camera::Trackball>();
			QAction * const freeflly  = addAction<Action::Camera::Freefly>();
			addSeparator();
			addAction<Action::Camera::Orient>();
			addAction<Action::Camera::Reset>();

			// TODO: where to do this?
			using namespace App::Controller::Camera;
			App::CONTROLLER_SYSTEM().onControllerEnabled += [ trackball, freeflly ]( const Name p_name )
			{
				if ( p_name == Trackball::NAME )
				{
					trackball->setChecked( true );
				}
				else if ( p_name == Freefly::NAME )
				{
					freeflly->setChecked( true );
				}
			};
		}

		virtual ~Camera() {}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
