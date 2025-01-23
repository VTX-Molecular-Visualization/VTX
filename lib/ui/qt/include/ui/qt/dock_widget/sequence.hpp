#ifndef __VTX_UI_QT_DOCK_WIDGET_SEQUENCE__
#define __VTX_UI_QT_DOCK_WIDGET_SEQUENCE__

#include "ui/qt/core/base_dock_widget.hpp"
#include <QTableWidget>
#include <app/component/chemistry/system.hpp>

namespace VTX::UI::QT::DockWidget
{
	/**
	 * @brief Display residues sequence for each loaded system.
	 * // TODO handle selection and color layout.
	 */
	class Sequence : public Core::BaseDockWidget<Sequence, 1, 1>
	{
	  public:
		Sequence( QWidget * );

	  private:
		using WidgetData = size_t;

		/**
		 * @brief Map top level items to system components.
		 */
		std::map<const QTableWidget * const, App::Component::Chemistry::System * const> _systemComponents;
	};

} // namespace VTX::UI::QT::DockWidget

#endif
