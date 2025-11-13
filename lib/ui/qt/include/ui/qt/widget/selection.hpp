#ifndef __VTX_UI_QT_WIDGET_SELECTION__
#define __VTX_UI_QT_WIDGET_SELECTION__

#include "ui/qt/widget/base_widget.hpp"
#include <QListWidget>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Widget displaying the current selection as a list.
	 */
	class Selection : public BaseWidget<Selection, QListWidget>
	{
	  public:
		Selection( QWidget * const p_parent = nullptr );

		/**
		 * @brief Refresh the whole QListWidget content.
		 */
		void refreshSelection();
	};
} // namespace VTX::UI::QT::Widget

#endif
