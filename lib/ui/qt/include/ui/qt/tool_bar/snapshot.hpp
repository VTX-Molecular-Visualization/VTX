#ifndef __VTX_UI_QT_TOOL_SNAPSHOT__
#define __VTX_UI_QT_TOOL_SNAPSHOT__

#include "ui/qt/base_widget.hpp"
#include <QToolBar>

namespace VTX::UI::QT::ToolBar
{

	class Snapshot : public BaseWidget<Snapshot, QToolBar>
	{
	  public:
		Snapshot( QWidget * p_parent ) : BaseWidget<Snapshot, QToolBar>( "Snapshot", p_parent )
		{
			addAction<Action::Snapshot::Snapshot>();
			addAction<Action::Snapshot::Export>();
		}

	  private:
	};

} // namespace VTX::UI::QT::ToolBar

#endif
