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
			addAction<UI::Action::Camera::Perspective>();
			addAction<UI::Action::Camera::Orthographic>();
			addSeparator();
			addAction<UI::Action::Camera::Trackball>();
			addAction<UI::Action::Camera::Freefly>();
			addSeparator();
			addAction<UI::Action::Camera::Orient>();
		}
		virtual ~Camera() {}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
