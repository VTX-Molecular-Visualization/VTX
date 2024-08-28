#ifndef __VTX_TOOL_EXAMPLE_WIDGET_MENU__
#define __VTX_TOOL_EXAMPLE_WIDGET_MENU__

#include "dialog.hpp"
#include "tool/example/actions.hpp"
#include <QMenu>
#include <qt/base_widget.hpp>

namespace VTX::Tool::Example::Widget
{

	class Menu : public UI::QT::BaseWidget<Menu, QMenu>
	{
	  public:
		Menu( QWidget * p_parent ) : BaseWidget<Menu, QMenu>( "My menu", p_parent )
		{
			// Connect to dialog.
			auto * action1 = addAction<Action::MyAction1>();
			connect(
				action1,
				&QAction::triggered,
				this,
				[]()
				{
					Dialog dialog;
					dialog.exec();
				}
			);

			addSeparator();
			addAction<Action::MyAction2>();
			addAction<Action::MyAction3>();
			addSeparator();
			addAction<Action::MyAction4>();
		}
		virtual ~Menu() {}

	  private:
	};

} // namespace VTX::Tool::Example::Widget

#endif
