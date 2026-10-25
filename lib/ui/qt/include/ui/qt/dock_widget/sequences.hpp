#ifndef __VTX_UI_QT_DOCK_WIDGET_SEQUENCES__
#define __VTX_UI_QT_DOCK_WIDGET_SEQUENCES__

#include "ui/qt/dock_widget/base_dock_widget.hpp"
#include "ui/qt/widget/sequence.hpp"
#include <app/ecs.hpp>
#include <app/events.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::DockWidget
{

	/**
	 * @brief Display residues sequence for each loaded system.
	 * // TODO handle selection and color layout.
	 */
	class Sequences : public BaseDockWidget<Sequences, true, true>
	{
	  public:
		Sequences( QWidget * );

	  private:
		std::unordered_map<Entity, QPointer<Widget::Sequence>> _mapSequenceWidgets;

		void _onSystemLoad( const App::Events::SystemLoad & );
		void _onSystemDestroy( Registry &, Entity );
		void _onUpdateSelection( Registry &, Entity );
		void _onColorsChanged( Registry &, Entity );
		void _onColorsPresetChanged( Registry &, Entity );

		VTX::Util::EventHub::ScopedConnection _systemLoadConnection;
		VTX::Util::EventHub::ScopedConnection _systemDestroyedConnection;
		VTX::Util::EventHub::ScopedConnection _selectionUpdatedConnection;
		VTX::Util::EventHub::ScopedConnection _colorLayoutConstructedConnection;
		VTX::Util::EventHub::ScopedConnection _colorLayoutUpdatedConnection;
		VTX::Util::EventHub::ScopedConnection _colorPresetUpdatedConnection;
	};

} // namespace VTX::UI::QT::DockWidget

#endif
