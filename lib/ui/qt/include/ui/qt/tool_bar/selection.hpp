#ifndef __VTX_UI_QT_TOOL_BAR_SELECTION__
#define __VTX_UI_QT_TOOL_BAR_SELECTION__

#include "ui/qt/widget/base_widget.hpp"
#include <QToolBar>

namespace VTX::UI::QT::ToolBar
{

	class Selection : public Widget::BaseWidget<Selection, QToolBar>
	{
	  public:
		Selection( QWidget * p_parent ) : BaseWidget( p_parent )
		{
			setWindowTitle( "Selection" );

			auto * lockAction = addAction<Action::Selection::Lock>();
			lockAction->setCheckable( true );
		}

	  private:
	};

} // namespace VTX::UI::QT::ToolBar

#endif
