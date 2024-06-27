#ifndef __VTX_UI_QT_TOOL_BAR_CAMERA__
#define __VTX_UI_QT_TOOL_BAR_CAMERA__

#include <QToolBar>

namespace VTX::UI::QT::ToolBar
{

	class Camera : public QToolBar
	{
	  public:
		Camera( QWidget * p_parent ) : QToolBar( "Camera", p_parent ) {
			addAction( new QAction( "Perspective" ) );
			addSeparator();
			addAction( new QAction( "Trackball" ) );
			addAction( new QAction( "Freefly" ) );
			addSeparator();
			addAction( new QAction( "Orient" ) );
			addAction( new QAction( "Reset" ) );
		}

		virtual ~Camera() {}

	  private:
	};

} // namespace VTX::UI::QT::ToolBar

#endif
