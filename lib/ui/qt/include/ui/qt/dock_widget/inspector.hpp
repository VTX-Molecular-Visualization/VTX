#ifndef __VTX_UI_QT_DOCK_WIDGET_INSPECTOR__
#define __VTX_UI_QT_DOCK_WIDGET_INSPECTOR__

#include "ui/qt/dock_widget/base_dock_widget.hpp"
#include "ui/qt/widget/selection.hpp"
#include <QComboBox>

namespace VTX::UI::QT::DockWidget
{
	/**
	 * @brief Inspector dock widget showing widgets about the current selection.
	 */
	class Inspector : public BaseDockWidget<Inspector>
	{
	  public:
		Inspector( QWidget * );

	  private:
		QPointer<Widget::Selection> _selectionListWidget;
		QPointer<QComboBox>			_cbPickingGranularity;
	};

} // namespace VTX::UI::QT::DockWidget

#endif
