#ifndef __VTX_UI_QT_DOCK_WIDGET_SEQUENCES__
#define __VTX_UI_QT_DOCK_WIDGET_SEQUENCES__

#include "ui/qt/dock_widget/scene.hpp"
#include <QDockWidget>
#include <QTableWidget>

namespace VTX::UI::QT::DockWidget
{

	/**
	 * @brief Display residues sequence for each loaded system.
	 * // TODO handle selection and color layout.
	 */
	class Sequences : public Core::BaseDockWidget<Sequences, 1, 1>
	{
	  public:
		Sequences( QWidget * );

	  private:
		using WidgetData = size_t;

		/**
		 * @brief Map top level items to system components.
		 */
		std::map<const QTableWidget * const, App::Component::Chemistry::System * const> _systemComponents;
	};

} // namespace VTX::UI::QT::DockWidget

#endif
