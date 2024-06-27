#ifndef __VTX_UI_QT_MENU_HELP__
#define __VTX_UI_QT_MENU_HELP__

#include <QMenu>

namespace VTX::UI::QT::Menu
{

	class Help : public QMenu
	{
	  public:
		Help( QWidget * p_parent ) : QMenu( "Help", p_parent )
		{
			addAction( new QAction( "Documentation" ) );
			addAction( new QAction( "Report a bug" ) );
			addAction( new QAction( "Check for updates" ) );
			addAction( new QAction( "About" ) );
		}
		virtual ~Help() {}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
