#ifndef __VTX_UI_QT_DOCK_WIDGET_SEQUENCES__
#define __VTX_UI_QT_DOCK_WIDGET_SEQUENCES__

#include "ui/qt/dock_widget/base_dock_widget.hpp"
#include <app/ecs.hpp>

namespace VTX::UI::QT::DockWidget
{

	/**
	 * @brief Display residues sequence for each loaded system.
	 * // TODO handle selection and color layout.
	 */
	class Sequences : public BaseDockWidget<Sequences, 1, 1>
	{
	  public:
		Sequences( QWidget * );

	  private:
		void _onConstructSystem( App::ECS::Registry &, App::ECS::Entity );
		void _onDestroySystem( App::ECS::Registry &, App::ECS::Entity );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
