#ifndef __VTX_TOOL_EXAMPLE_WIDGET_MY_TOOL_BAR__
#define __VTX_TOOL_EXAMPLE_WIDGET_MY_TOOL_BAR__

#include "tool/example/actions.hpp"
#include <QGridLayout>
#include <QToolBar>
#include <ui/qt/widget/base_widget.hpp>

namespace VTX::Tool::Example::Widget
{

	class MyToolBar : public UI::QT::Widget::BaseWidget<MyToolBar, QToolBar>
	{
	  public:
		MyToolBar( QWidget * p_parent ) : BaseWidget( p_parent )
		{
			setWindowTitle( "My ToolBar" );

			addAction( Action::MY_ACTION_1 );
			addAction( Action::MY_ACTION_2 );
			addAction( Action::MY_ACTION_3 );
			addAction( Action::MY_ACTION_4 );
		}

		virtual ~MyToolBar() {}

	  private:
	};

} // namespace VTX::Tool::Example::Widget

#endif
