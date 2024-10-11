#ifndef __VTX_TOOL_EXAMPLE_WIDGET_MY_TOOL_BAR__
#define __VTX_TOOL_EXAMPLE_WIDGET_MY_TOOL_BAR__

#include "tool/example/actions.hpp"
#include <QGridLayout>
#include <QToolBar>
#include <ui/qt/base_widget.hpp>

namespace VTX::Tool::Example::Widget
{

	class MyToolBar : public UI::QT::Core::BaseWidget<MyToolBar, QToolBar>
	{
	  public:
		MyToolBar( QWidget * p_parent ) : BaseWidget<MyToolBar, QToolBar>( "My toolbar", p_parent )
		{
			addAction<Action::MyAction1>();
			addAction<Action::MyAction2>();
			addAction<Action::MyAction3>();
			addAction<Action::MyAction4>();
		}

		virtual ~MyToolBar() {}

	  private:
	};

} // namespace VTX::Tool::Example::Widget

#endif
