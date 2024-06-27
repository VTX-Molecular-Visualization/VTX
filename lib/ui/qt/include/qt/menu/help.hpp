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
			using namespace Helper;

			addQAction( this, { "Documentation", "Open documentation in your browser" } );
			addQAction( this, { "Report a bug", "Open " } );
			addQAction( this, { "Check for updates", "Check our server for a new version" } );
			addQAction( this, { "About" } );
		}

		virtual ~Help() {}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
