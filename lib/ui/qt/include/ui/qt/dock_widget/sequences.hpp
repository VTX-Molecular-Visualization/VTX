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
	  public:
		Sequences( QWidget * );

	  private:
		std::unordered_map<App::Entity, QPointer<Widget::Sequence>> _mapSequenceWidgets;

		void _onSystemLoad( const App::Events::SystemLoad & );
		void _onSystemDestroy( App::Registry &, App::Entity );
		void _onUpdateSelection( App::Registry &, App::Entity );
		void _onColorsChanged( App::Registry &, App::Entity );
		void _onColorsPresetChanged( App::Registry &, App::Entity );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
