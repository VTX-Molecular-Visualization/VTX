#ifndef __VTX_UI_QT_MENU_HELP__
#define __VTX_UI_QT_MENU_HELP__

#include "qt/base_widget.hpp"
#include "qt/helper.hpp"
#include <QMenu>

namespace VTX::UI::QT::Menu
{

	class Help : public BaseWidget<Help, QMenu>
	{
	  public:
		Help( QWidget * p_parent ) : BaseWidget<Help, QMenu>( "Help", p_parent )
		{
			addAction<Action::Help::Documentation>();
			addAction<Action::Help::Report>();
			addAction<Action::Help::CheckUpdates>();
			addAction<Action::Help::About>();
		}

		virtual ~Help() {}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
