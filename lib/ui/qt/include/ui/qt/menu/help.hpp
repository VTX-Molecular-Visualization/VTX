#ifndef __VTX_UI_QT_MENU_HELP__
#define __VTX_UI_QT_MENU_HELP__

#include "ui/qt/helper.hpp"
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

			addAction( Action::Help::DOCUMENTATION );
			addAction( Action::Help::REPORT );
			addAction( Action::Help::CHECK_UPDATES );
			// addAction( Action::Help::ABOUT );
		}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
