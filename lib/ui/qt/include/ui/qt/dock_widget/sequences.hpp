#ifndef __VTX_UI_QT_DOCK_WIDGET_SEQUENCES__
#define __VTX_UI_QT_DOCK_WIDGET_SEQUENCES__

#include "ui/qt/dock_widget/base_dock_widget.hpp"
#include "ui/qt/widget/sequence.hpp"
#include <app/ecs.hpp>
#include <app/events.hpp>

namespace VTX::UI::QT::DockWidget
{

	/**
	 * @brief Display residues sequence for each loaded system.
	 * // TODO handle selection and color layout.
	 */
	class Sequences : public BaseDockWidget<Sequences, 1, 1>
	{
		Q_OBJECT
	  public:
		Sequences( QWidget * );

	  private:
		std::unordered_map<App::ECS::Entity, Widget::Sequence *> _mapSequencesWidgets;

		void _onSystemLoad( const App::Events::SystemLoad & );
		void _onDestroySystem( App::ECS::Registry &, App::ECS::Entity );
		void _onUpdateSelection( App::ECS::Registry &, App::ECS::Entity );
		void _onColorsChanged( App::ECS::Registry &, App::ECS::Entity );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
