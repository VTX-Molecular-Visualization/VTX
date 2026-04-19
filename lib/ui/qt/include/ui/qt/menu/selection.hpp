#ifndef __VTX_UI_QT_MENU_SELECTION__
#define __VTX_UI_QT_MENU_SELECTION__

#include "ui/qt/widget/base_widget.hpp"
#include <QMenu>

namespace VTX::UI::QT::Menu
{
	/**
	 * @brief Menu for selection actions.
	 * This menu is dynamically filled with actions based on the current selection when opened.
	 */
	class Selection : public Widget::BaseWidget<Selection, QMenu>
	{
	  public:
		Selection( QWidget * p_parent );

	  private:
		void _refresh();
	};

} // namespace VTX::UI::QT::Menu

#endif
