#ifndef __VTX_UI_QT_MENU_HELP__
#define __VTX_UI_QT_MENU_HELP__

#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include <QMenu>

namespace VTX::UI::QT::Menu
{

	class Help : public Widget::BaseWidget<Help, QMenu>
	{
	  public:
		Help( QWidget * p_parent ) : BaseWidget( p_parent )
		{
			setTitle( "Help" );

			UI_ACTIONS().addTo( *this, Action::Help::DOCUMENTATION );
			UI_ACTIONS().addTo( *this, Action::Help::REPORT );
			UI_ACTIONS().addTo( *this, Action::Help::CHECK_UPDATES );
			// UI_ACTIONS().addTo( *this, Action::Help::ABOUT );
		}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
