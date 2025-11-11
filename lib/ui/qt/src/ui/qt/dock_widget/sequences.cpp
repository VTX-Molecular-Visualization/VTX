#include "ui/qt/dock_widget/sequences.hpp"
#include "ui/qt/selection_model.hpp"
#include "ui/qt/services.hpp"
#include <app/events.hpp>
#include <app/services.hpp>

namespace VTX::UI::QT::DockWidget
{
	Sequences::Sequences( QWidget * p_parent ) : BaseDockWidget( "Sequence", p_parent )
	{
		setAllowedAreas( Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );

		App::REG().on_construct<Core::Struct::System>().connect<&Sequences::_onConstructSystem>( this );
		App::REG().on_destroy<Core::Struct::System>().connect<&Sequences::_onDestroySystem>( this );
	}

	void Sequences::_onConstructSystem( App::ECS::Registry & p_r, App::ECS::Entity p_e )
	{
		auto & system		  = p_r.get<Core::Struct::System>( p_e );
		auto * sequenceWidget = new Widget::Sequence( p_e, this );

		// Create Widget.
		assert( not _mapSequencesWidgets.contains( p_e ) );
		_mapSequencesWidgets.emplace( p_e, sequenceWidget );
		_layout->addWidget( sequenceWidget );

		// Refresh widget when selection changed.
		auto & selectionModel = SELECTION();
		connect(
			&selectionModel, &QItemSelectionModel::selectionChanged, [ sequenceWidget ] { sequenceWidget->update(); }
		);
	}

	void Sequences::_onDestroySystem( App::ECS::Registry & p_r, App::ECS::Entity p_e )
	{
		// Remove from map and delete widget.
		assert( _mapSequencesWidgets.contains( p_e ) );
		_mapSequencesWidgets[ p_e ]->deleteLater();
		_mapSequencesWidgets.erase( p_e );
	}

} // namespace VTX::UI::QT::DockWidget
