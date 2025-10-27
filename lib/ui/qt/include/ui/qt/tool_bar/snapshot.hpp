#ifndef __VTX_UI_QT_TOOL_SNAPSHOT__
#define __VTX_UI_QT_TOOL_SNAPSHOT__

#include "ui/qt/services.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include "ui/qt/widget/main_window.hpp"
#include <QToolBar>

namespace VTX::UI::QT::ToolBar
{

	class Snapshot : public Widget::BaseWidget<Snapshot, QToolBar>
	{
	  public:
		Snapshot( QWidget * p_parent ) : BaseWidget( "Snapshot", p_parent )
		{
			addAction( MAIN_WINDOW().getAction<Action::Snapshot::Snapshot>() );
			addAction( MAIN_WINDOW().getAction<Action::Snapshot::Export>() );
		}

	  private:
	};

} // namespace VTX::UI::QT::ToolBar

#endif
