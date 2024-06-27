#ifndef __VTX_UI_QT_MENU_FILE__
#define __VTX_UI_QT_MENU_FILE__

#include <QMenu>

namespace VTX::UI::QT::Menu
{

	class File : public QMenu
	{
	  public:
		File( QWidget * p_parent ) : QMenu( "File", p_parent )
		{
			addAction( new QAction( "New" ) );
			addSeparator();
			addAction( new QAction( "Open" ) );
			addAction( new QAction( "Open recent" ) );
			addAction( new QAction( "Save" ) );
			addAction( new QAction( "Save as..." ) );
			addSeparator();
			addAction( new QAction( "Import" ) );
			addAction( new QAction( "Export" ) );
			addSeparator();
			addAction( new QAction( "Quit" ) );
		}
		virtual ~File() {}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
