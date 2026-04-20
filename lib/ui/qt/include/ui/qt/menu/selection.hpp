#ifndef __VTX_UI_QT_MENU_SELECTION__
#define __VTX_UI_QT_MENU_SELECTION__

#include "ui/qt/menu/color_scheme.hpp"
#include "ui/qt/menu/representation.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include <QMenu>
#include <QPointer>

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
		QPointer<ColorScheme>	 _colorSchemeMenu;
		QPointer<Representation> _representationMenu;

		void _refresh();
	};

} // namespace VTX::UI::QT::Menu

#endif
