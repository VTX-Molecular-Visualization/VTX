#ifndef __VTX_UI_QT_TOOL_BAR_CAMERA__
#define __VTX_UI_QT_TOOL_BAR_CAMERA__

#include "ui/qt/widget/base_widget.hpp"
#include <QToolBar>

namespace VTX::UI::QT::ToolBar
{

	class Camera : public Widget::BaseWidget<Camera, QToolBar>
	{
	  public:
		Camera( QWidget * p_parent ) : BaseWidget( p_parent )
		{
			setWindowTitle( "Camera" );

			addAction( Action::Camera::PERSPECTIVE );
			addAction( Action::Camera::ORTHOGRAPHIC );
			addSeparator();
			addAction( Action::Camera::TRACKBALL );
			addAction( Action::Camera::FREEFLY );
			addSeparator();
			addAction( Action::Camera::ORIENT );
			addAction( Action::Camera::RESET );
		}

	  private:
	};

} // namespace VTX::UI::QT::ToolBar

#endif
