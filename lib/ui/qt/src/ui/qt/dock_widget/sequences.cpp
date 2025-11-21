#include "ui/qt/dock_widget/sequences.hpp"
#include "ui/qt/selection_model.hpp"
#include "ui/qt/services.hpp"
#include <app/events.hpp>
#include <app/services.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::DockWidget
{
	Sequences::Sequences( QWidget * p_parent ) : BaseDockWidget( p_parent )
	{
		setWindowTitle( "Sequence" );
		setAllowedAreas( Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );

		App::HUB().connect<App::Events::SystemLoad, &Sequences::_onSystemLoad>( this );
		App::REG().on_destroy<Core::Struct::System>().connect<&Sequences::_onDestroySystem>( this );

		// Refresh widget when selection changed.
		App::HUB().connect<App::Events::SelectionChange, &Sequences::_onSelectionChange>( this );
	}

	void Sequences::_onSystemLoad( const App::Events::SystemLoad & p_e )
	{
		const auto	 entity			= p_e.system;
		const auto & system			= App::REG().get<Core::Struct::System>( entity );
		auto *		 sequenceWidget = new Widget::Sequence( entity, this );

		// Create Widget.
		assert( not _mapSequencesWidgets.contains( entity ) );
		_mapSequencesWidgets.emplace( entity, sequenceWidget );
		_layout->addWidget( sequenceWidget );
	}

	void Sequences::_onDestroySystem( App::ECS::Registry & p_r, App::ECS::Entity p_e )
	{
		// Remove from map and delete widget.
		assert( _mapSequencesWidgets.contains( p_e ) );
		_mapSequencesWidgets[ p_e ]->deleteLater();
		_mapSequencesWidgets.erase( p_e );
	}

	void Sequences::_onSelectionChange( const App::Events::SelectionChange & p_e )
	{
		const auto entity = p_e.system;
		assert( _mapSequencesWidgets.contains( entity ) );
		_mapSequencesWidgets[ entity ]->viewport()->update();
	}

} // namespace VTX::UI::QT::DockWidget
