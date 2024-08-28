#ifndef __VTX_TOOL_EXAMPLE_WIDGET_TOOL_BAR__
#define __VTX_TOOL_EXAMPLE_WIDGET_TOOL_BAR__

#include "tool/example/actions.hpp"
#include <QGridLayout>
#include <QToolBar>
#include <qt/base_widget.hpp>

namespace VTX::Tool::Example::Widget
{

	class ToolBar : public UI::QT::BaseWidget<ToolBar, QToolBar>
	{
	  public:
		ToolBar( QWidget * p_parent ) : BaseWidget<ToolBar, QToolBar>( "My toolbar", p_parent )
		{
			setLayout( new QGridLayout() );

			addAction<Action::MyAction1>();
			addAction<Action::MyAction2>();
			addAction<Action::MyAction3>();
			addAction<Action::MyAction4>();
		}

		virtual ~ToolBar() {}

	  private:
	};

} // namespace VTX::Tool::Example::Widget

#endif
