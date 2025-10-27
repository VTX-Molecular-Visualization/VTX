#ifndef __VTX_UI_QT_MENU_SELECTION__
#define __VTX_UI_QT_MENU_SELECTION__

#include "ui/qt/services.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include "ui/qt/widget/main_window.hpp"
#include <QMenu>

namespace VTX::UI::QT::Menu
{
	class Selection : public Widget::BaseWidget<Selection, QMenu>
	{
	  public:
		Selection( QWidget * p_parent ) : BaseWidget( "Selection", p_parent )
		{
			addAction( MAIN_WINDOW().getAction<Action::Selection::SetGranularitySystem>() );
			addAction( MAIN_WINDOW().getAction<Action::Selection::SetGranularityChain>() );
			addAction( MAIN_WINDOW().getAction<Action::Selection::SetGranularityResidue>() );
			addAction( MAIN_WINDOW().getAction<Action::Selection::SetGranularityAtom>() );
		}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
