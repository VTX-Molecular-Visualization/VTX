#ifndef __VTX_UI_QT_MENU_HELP__
#define __VTX_UI_QT_MENU_HELP__

#include "ui/qt/helper.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include "ui/qt/widget/main_window.hpp"
#include <QMenu>

namespace VTX::UI::QT::Menu
{

	class Help : public Widget::BaseWidget<Help, QMenu>
	{
	  public:
		Help( QWidget * p_parent ) : BaseWidget( "Help", p_parent )
		{
			addAction( MAIN_WINDOW().getAction<Action::Help::Documentation>() );
			addAction( MAIN_WINDOW().getAction<Action::Help::Report>() );
			addAction( MAIN_WINDOW().getAction<Action::Help::CheckUpdates>() );
			addAction( MAIN_WINDOW().getAction<Action::Help::About>() );
		}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
