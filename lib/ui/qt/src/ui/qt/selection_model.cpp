#include "ui/qt/selection_model.hpp"
#include "ui/qt/model.hpp"
#include <app/action/action_manager.hpp>
#include <app/action/selection.hpp>
#include <app/services.hpp>
#include <app/system/uid.hpp>
#include <core/struct/system.hpp>
#include <util/chrono.hpp>

namespace VTX::UI::QT
{

	SelectionModel::SelectionModel( QAbstractItemModel * p_model, QObject * p_parent ) :
		QItemSelectionModel( p_model, p_parent )
	{
		// QT -> App.
		connect( this, &QItemSelectionModel::selectionChanged, this, &SelectionModel::_selectionChanged );

		// App -> QT.
		App::HUB().connect<App::Events::SelectionChange, &SelectionModel::_onSelectionChange>( this );
	}

	// TODO: optimize and factorize.
	void SelectionModel::_selectionChanged( const QItemSelection & p_selected, const QItemSelection & p_deselected )
	{
		VTX_DEBUG( "SelectionModel::_selectionChanged" );

		using namespace App;

		Util::Chrono timer;
		timer.start();

		const Model *										  model = static_cast<const Model *>( this->model() );
		const std::unordered_map<RootUID, App::ECS::Entity> & mapRootToEntity = model->getMapRootToEntity();

		// Create range lists per entity.
		using MapUIDRangeList = std::unordered_map<RootUID, Core::Struct::IndexRangeList>;
		MapUIDRangeList											  deselected;
		MapUIDRangeList											  selected;
		std::unordered_map<RootUID, const Core::Struct::System *> systems;

		for ( auto & [ uid, _ ] : mapRootToEntity )
		{
			deselected[ uid ] = {};
			selected[ uid ]	  = {};
			systems[ uid ]	  = &REG().get<Core::Struct::System>( mapRootToEntity.at( uid ) );
		}

		// Deselected items.
		if ( not p_deselected.isEmpty() )
		{
			for ( const QItemSelectionRange & range : p_deselected )
			{
				for ( int row = range.top(); row <= range.bottom(); ++row )
				{
					QModelIndex		   index = range.model()->index( row, 0, range.parent() );
					App::Scene::E_ITEM item;
					RootUID			   rootIndex;
					Index			   localIndex;
					Model::unpack( index.internalId(), item, rootIndex, localIndex );

					auto & system = *systems[ rootIndex ];
					auto & rl	  = deselected[ rootIndex ];

					switch ( item )
					{
					case App::Scene::E_ITEM::SYSTEM: rl.addRange( system.getAtomRange() ); break;
					case App::Scene::E_ITEM::CHAIN: rl.addRange( system.getChainAtomRange( localIndex ) ); break;
					case App::Scene::E_ITEM::RESIDUE: rl.addRange( system.getResidueAtomRange( localIndex ) ); break;
					case App::Scene::E_ITEM::ATOM: rl.addValue( localIndex ); break;
					default: break;
					}
				}
			}

			for ( const auto & [ uid, rl ] : deselected )
			{
				if ( rl.count() == 0 )
				{
					continue;
				}

				const ECS::Entity ent = mapRootToEntity.at( uid );
				ACTION().execute<Action::Selection::SetSelected<Scene::E_ITEM::ATOM>>( ent, rl, false );
			}
		}

		// Selected items.
		if ( not p_selected.isEmpty() )
		{
			for ( const QItemSelectionRange & range : p_selected )
			{
				for ( int row = range.top(); row <= range.bottom(); ++row )
				{
					QModelIndex		   index = range.model()->index( row, 0, range.parent() );
					App::Scene::E_ITEM item;
					RootUID			   rootIndex;
					Index			   localIndex;
					Model::unpack( index.internalId(), item, rootIndex, localIndex );

					auto & system = *systems[ rootIndex ];
					auto & rl	  = selected[ rootIndex ];

					switch ( item )
					{
					case App::Scene::E_ITEM::SYSTEM: rl.addRange( system.getAtomRange() ); break;
					case App::Scene::E_ITEM::CHAIN: rl.addRange( system.getChainAtomRange( localIndex ) ); break;
					case App::Scene::E_ITEM::RESIDUE: rl.addRange( system.getResidueAtomRange( localIndex ) ); break;
					case App::Scene::E_ITEM::ATOM: rl.addValue( localIndex ); break;
					default: break;
					}
				}
			}

			for ( const auto & [ uid, rl ] : selected )
			{
				if ( rl.count() == 0 )
				{
					continue;
				}

				const ECS::Entity ent = mapRootToEntity.at( uid );
				ACTION().execute<Action::Selection::SetSelected<Scene::E_ITEM::ATOM>>( ent, rl, true );
			}
		}

		VTX_DEBUG(
			"SelectionModel::_selectionChanged - done ({})", Util::String::durationToStr( timer.elapsedTime() )
		);
	}

	void SelectionModel::_onSelectionChange( const App::Events::SelectionChange & p_e )
	{
		using namespace App;
		// Block recursive selection change.

		auto &		 reg	   = REG();
		const auto	 entity	   = p_e.system;
		const auto & selection = reg.get<System::Selection>( entity );
		const auto & uid	   = reg.get<System::UID>( entity );

		// QSignalBlocker blocker( this );
		// clear();
	}
} // namespace VTX::UI::QT
