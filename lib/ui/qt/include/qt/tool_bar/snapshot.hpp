#ifndef __VTX_UI_QT_TOOL_SNAPSHOT__
#define __VTX_UI_QT_TOOL_SNAPSHOT__

#include "qt/helper.hpp"
#include <QToolBar>

namespace VTX::UI::QT::ToolBar
{

	class Snapshot : public QToolBar
	{
	  public:
		Snapshot( QWidget * p_parent ) : QToolBar( "Camera", p_parent )
		{
			using namespace Helper;

			addQAction( this, { "Snapshot", "Save current image" } );
			addQAction( this, { "Export image", "Open dialog to export image" } );
		}

		virtual ~Snapshot() {}

	  private:
	};

} // namespace VTX::UI::QT::ToolBar

#endif
