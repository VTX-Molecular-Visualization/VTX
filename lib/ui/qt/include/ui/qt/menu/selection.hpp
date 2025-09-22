#ifndef __VTX_UI_QT_MENU_SELECTION__
#define __VTX_UI_QT_MENU_SELECTION__

#include "ui/qt/core/base_widget.hpp"
#include <QMenu>

namespace VTX::UI::QT::Menu
{
	class Selection : public Core::BaseWidget<Selection, QMenu>
	{
	  public:
		Selection( QWidget * p_parent ) : BaseWidget<Selection, QMenu>( "Selection", p_parent )
		{
			addAction<Action::Selection::SetGranularitySystem>();
			addAction<Action::Selection::SetGranularityChain>();
			addAction<Action::Selection::SetGranularityResidue>();
			addAction<Action::Selection::SetGranularityAtom>();
		}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
