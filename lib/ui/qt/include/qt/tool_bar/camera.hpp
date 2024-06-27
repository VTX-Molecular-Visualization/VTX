#ifndef __VTX_UI_QT_TOOL_BAR_CAMERA__
#define __VTX_UI_QT_TOOL_BAR_CAMERA__

#include <QToolBar>

namespace VTX::UI::QT::ToolBar
{

	class Camera : public QToolBar
	{
	  public:
		Camera( QWidget * p_parent ) : QToolBar( "Camera", p_parent ) {}
		virtual ~Camera() {}

	  private:
	};

} // namespace VTX::UI::QT::ToolBar

#endif
