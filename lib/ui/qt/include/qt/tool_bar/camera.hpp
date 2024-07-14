#ifndef __VTX_UI_QT_TOOL_BAR_CAMERA__
#define __VTX_UI_QT_TOOL_BAR_CAMERA__

#include "qt/base_widget.hpp"
#include <QToolBar>

namespace VTX::UI::QT::ToolBar
{

	class Camera : public BaseWidget<Camera, QToolBar>
	{
	  public:
		Camera( QWidget * p_parent ) : BaseWidget<Camera, QToolBar>( "Camera", p_parent )
		{
			addAction<UI::Action::Camera::Perspective>();
			addAction<UI::Action::Camera::Orthographic>();
			addSeparator();
			addAction<UI::Action::Camera::Trackball>();
			addAction<UI::Action::Camera::Freefly>();
			addSeparator();
			addAction<UI::Action::Camera::Orient>();
			addAction<UI::Action::Camera::Reset>();
		}

		virtual ~Camera() {}

	  private:
	};

} // namespace VTX::UI::QT::ToolBar

#endif
