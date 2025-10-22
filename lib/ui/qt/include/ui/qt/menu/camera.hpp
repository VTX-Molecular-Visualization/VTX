#ifndef __VTX_UI_QT_MENU_CAMERA__
#define __VTX_UI_QT_MENU_CAMERA__

#include "ui/qt/widget/base_widget.hpp"
#include <QMenu>

namespace VTX::UI::QT::Menu
{
	class Camera : public Widget::BaseWidget<QMenu>
	{
	  public:
		Camera( QWidget * p_parent ) : BaseWidget( "Camera", p_parent )
		{
			addAction<Action::Camera::Perspective>();
			addAction<Action::Camera::Orthographic>();
			addSeparator();
			addAction<Action::Camera::Trackball>();
			addAction<Action::Camera::Freefly>();
			addSeparator();
			addAction<Action::Camera::Orient>();
			addAction<Action::Camera::Reset>();
		}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
