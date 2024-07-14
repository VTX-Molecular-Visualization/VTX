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
			addAction<UI::Action::Help::Documentation>();
			addAction<UI::Action::Help::Report>();
			addAction<UI::Action::Help::CheckUpdates>();
			addAction<UI::Action::Help::About>();
		}

		virtual ~Help() {}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
