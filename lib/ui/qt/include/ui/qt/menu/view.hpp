#ifndef __VTX_UI_QT_MENU_VIEW__
#define __VTX_UI_QT_MENU_VIEW__

#include "ui/qt/base_widget.hpp"
#include "ui/qt/helper.hpp"
#include <QMenu>

namespace VTX::UI::QT::Menu
{

	class View : public BaseWidget<View, QMenu>
	{
	  public:
		View( QWidget * p_parent ) : BaseWidget<View, QMenu>( "View", p_parent )
		{
			/*
			addQAction( this, { "Console", "Show/hide console window" } );
			addQAction( this, { "Inspector", "Show/hide inspector window" } );
			addQAction( this, { "Options", "Show/hide options window" } );
			addQAction( this, { "Renderer", "Show/hide renderer window" } );
			addQAction( this, { "Render settings", "Show/hide render settings window" } );
			addQAction( this, { "Representations", "Show/hide representations window" } );
			addQAction( this, { "Sequence", "Show/hide sequence window" } );
			addQAction( this, { "Scene", "Show/hide scene window" } );
			addSeparator();
			addQAction( this, { "Fullscreen" } );

			// Set action checkables.
			for ( QAction * action : actions() )
			{
				action->setCheckable( true );
				action->setChecked( true );
			}
			*/
		}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
