#ifndef VTX_UI_QT_MENU_VIEW_HPP
#define VTX_UI_QT_MENU_VIEW_HPP

#include "ui/qt/helper.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include <QMenu>

namespace VTX::UI::QT::Menu
{

	class View : public Widget::BaseWidget<View, QMenu>
	{
	  public:
		View( QWidget * const p_parent ) : BaseWidget( p_parent )
		{
			setTitle( "View" );
			connect( this, &QMenu::aboutToShow, this, [ this ]() { MAIN_WINDOW().populateViewMenu( *this ); } );
		}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
