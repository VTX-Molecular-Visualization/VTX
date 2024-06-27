#ifndef __VTX_UI_QT_MENU_VIEW__
#define __VTX_UI_QT_MENU_VIEW__

#include <QMenu>

namespace VTX::UI::QT::Menu
{

	class View : public QMenu
	{
	  public:
		View( QWidget * p_parent ) : QMenu( "View", p_parent )
		{
			addAction( new QAction( "Console" ) );
			addAction( new QAction( "Inspector" ) );
			addAction( new QAction( "Options" ) );
			addAction( new QAction( "Renderer" ) );
			addAction( new QAction( "Render settings" ) );
			addAction( new QAction( "Representations" ) );
			addAction( new QAction( "Sequence" ) );
			addAction( new QAction( "Scene" ) );
			addSeparator();
			QMenu * viewOverlayMenu = addMenu( "Overlay" );
			addSeparator();
			addAction( new QAction( "Fullscreen" ) );

			// Set action checkables.
			for ( QAction * action : actions() )
			{
				action->setCheckable( true );
				action->setChecked( true );
				action->setStatusTip( "Show/hide " + action->text().toLower() );
			}
		}

		virtual ~View() {}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
