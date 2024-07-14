#ifndef __VTX_UI_QT_TOOL_SNAPSHOT__
#define __VTX_UI_QT_TOOL_SNAPSHOT__

#include "qt/base_widget.hpp"
#include <QToolBar>

namespace VTX::UI::QT::ToolBar
{

	class Snapshot : public BaseWidget<Snapshot, QToolBar>
	{
	  public:
		Snapshot( QWidget * p_parent ) : BaseWidget<Snapshot, QToolBar>( "Snapshot", p_parent )
		{
			addAction<UI::Action::Snapshot::Snapshot>();
			addAction<UI::Action::Snapshot::Export>();
		}

		virtual ~Snapshot() {}

	  private:
	};

} // namespace VTX::UI::QT::ToolBar

#endif
