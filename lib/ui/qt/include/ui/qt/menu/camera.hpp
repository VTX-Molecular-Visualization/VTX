#ifndef __VTX_UI_QT_MENU_CAMERA__
#define __VTX_UI_QT_MENU_CAMERA__

#include "ui/qt/services.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include "ui/qt/widget/main_window.hpp"
#include <QMenu>

namespace VTX::UI::QT::Menu
{
	class Camera : public Widget::BaseWidget<Camera, QMenu>
	{
	  public:
		Camera( QWidget * p_parent ) : BaseWidget( "Camera", p_parent )
		{
			addAction( MAIN_WINDOW().getAction<Action::Camera::Perspective>() );
			addAction( MAIN_WINDOW().getAction<Action::Camera::Orthographic>() );
			addSeparator();
			addAction( MAIN_WINDOW().getAction<Action::Camera::Trackball>() );
			addAction( MAIN_WINDOW().getAction<Action::Camera::Freefly>() );
			addSeparator();
			addAction( MAIN_WINDOW().getAction<Action::Camera::Orient>() );
			addAction( MAIN_WINDOW().getAction<Action::Camera::Reset>() );
		}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
