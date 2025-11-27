#include "ui/qt/selection_model.hpp"
#include "ui/qt/model.hpp"
#include <app/action/action_manager.hpp>
#include <app/action/selection.hpp>
#include <app/helper/system.hpp>
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
		App::REG().on_update<App::System::Selection>().connect<&SelectionModel::_onUpdateSelection>( this );
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

	// TODO: update only for incoming entity!
	// (blocked by QItemSelectionModel::ClearAndSelect)
	void SelectionModel::_onUpdateSelection( App::ECS::Registry &, App::ECS::Entity p_e )
	{
		using namespace App;
		using namespace App::Scene;

		auto &													reg		 = REG();
		const auto												entities = reg.view<App::System::Selection>();
		const auto *											model	 = static_cast<const Model *>( this->model() );
		const std::unordered_map<RootUID, const Model::Row *> & mapRootToRows = model->getMapRootToRows();

		QSignalBlocker blocker( this );
		QItemSelection qSelection;

		for ( auto entity : entities )
		{
			const auto &  system	= reg.get<Core::Struct::System>( entity );
			const auto &  selection = reg.get<System::Selection>( entity );
			const auto &  uid		= reg.get<System::UID>( entity );
			const RootUID rootUID	= uid.system;

			if ( Helper::System::isFullySelected<E_ITEM::SYSTEM>( entity ) )
			{
				const QModelIndex index
					= model->makeIndex( mapRootToRows.at( rootUID )->position, E_ITEM::SYSTEM, rootUID, 0 );
				qSelection.select( index, index );
				continue;
			}
			else if ( Helper::System::isSelected<E_ITEM::SYSTEM>( entity ) == false )
			{
				continue;
			}

			// Chains.
			for ( Index chain = 0; chain < system.getChainCount(); ++chain )
			{
				QString chainName = QString::fromStdString( system.getChainName( chain ) );
				if ( Helper::System::isFullySelected<E_ITEM::CHAIN>( entity, chain ) )
				{
					const QModelIndex index = model->makeIndex( chain, E_ITEM::CHAIN, rootUID, chain );
					qSelection.select( index, index );
					continue;
				}
				else if ( Helper::System::isSelected<E_ITEM::CHAIN>( entity, chain ) == false )
				{
					continue;
				}

				// Residues.
				for ( Index residue : system.getChainResidueRange( chain ) )
				{
					QString residueName = QString::fromStdString( system.getResidueName( residue ) );
					if ( Helper::System::isFullySelected<E_ITEM::RESIDUE>( entity, residue ) )
					{
						const QModelIndex index = model->makeIndex(
							residue - system.getChainFirstResidue( chain ), E_ITEM::RESIDUE, rootUID, residue
						);
						qSelection.select( index, index );
						continue;
					}
					else if ( Helper::System::isSelected<E_ITEM::RESIDUE>( entity, residue ) == false )
					{
						continue;
					}

					// Atoms.
					for ( Index atom : system.getResidueAtomRange( residue ) )
					{
						if ( Helper::System::isSelected<E_ITEM::ATOM>( entity, atom ) )
						{
							const QModelIndex index = model->makeIndex(
								atom - system.getResidueFirstAtom( residue ), E_ITEM::ATOM, rootUID, atom
							);
							qSelection.select( index, index );
						}
					}
				}
			}
		}

		// Apply.
		select( qSelection, QItemSelectionModel::ClearAndSelect );
	}

} // namespace VTX::UI::QT
