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
			addAction<Action::Camera::CameraPerspective>();
			addAction<Action::Camera::CameraOrthographic>();
			addSeparator();
			addAction<Action::Camera::Trackball>();
			addAction<Action::Camera::Freefly>();
			addSeparator();
			addAction<Action::Camera::Orient>();
			addAction<Action::Camera::Reset>();
		}

		virtual ~Camera() {}

	  private:
	};

} // namespace VTX::UI::QT::ToolBar

#endif
