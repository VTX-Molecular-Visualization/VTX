#ifndef __VTX_UI_QT_TOOL_BAR_CAMERA__
#define __VTX_UI_QT_TOOL_BAR_CAMERA__

#include "qt/helper.hpp"
#include <QToolBar>

namespace VTX::UI::QT::ToolBar
{

	class Camera : public QToolBar
	{
	  public:
		Camera( QWidget * p_parent ) : QToolBar( "Camera", p_parent )
		{
			using namespace Helper;

			addQAction( this, { "Perspective", "Change projection mode (perspective/orthographic)" } );
			addSeparator();
			addQAction( this, { "Trackball", "Use Trackball controller" } );
			addQAction( this, { "Freefly", "Use Freefly controller" } );
			addSeparator();
			addQAction( this, { "Orient", "Orient camera on selection" } );
			addQAction( this, { "Reset", "Reset camera" } );
		}

		virtual ~Camera() {}

	  private:
	};

} // namespace VTX::UI::QT::ToolBar

#endif
