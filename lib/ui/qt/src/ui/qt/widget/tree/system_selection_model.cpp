#include "ui/qt/widget/tree/system_selection_model.hpp"
#include "ui/qt/widget/tree/system_model.hpp"
#include <app/action/action_manager.hpp>
#include <app/action/selection.hpp>
#include <app/helper/system.hpp>
#include <app/services.hpp>
#include <app/system/uid.hpp>
#include <core/struct/system.hpp>

namespace VTX::UI::QT::Widget::Tree
{

	SystemSelectionModel::SystemSelectionModel(
		const App::ECS::Entity p_system,
		QAbstractItemModel *   p_model,
		QObject *			   p_parent
	) : _system( p_system ), QItemSelectionModel( p_model, p_parent )
	{
		connect( this, &QItemSelectionModel::selectionChanged, this, &SystemSelectionModel::_selectionChanged );
	}

	void SystemSelectionModel::_selectionChanged(
		const QItemSelection & p_selected,
		const QItemSelection & p_deselected
	)
	{
		VTX_DEBUG( "SelectionModel::_selectionChanged" );

		using namespace App;
		using namespace Core::Struct;

		const SystemModel & model  = *static_cast<const SystemModel *>( this->model() );
		const auto &		system = REG().get<Core::Struct::System>( _system );

		// Deselected items.
		Core::Struct::IndexRangeList deselected;
		if ( not p_deselected.isEmpty() )
		{
			for ( const QItemSelectionRange & range : p_deselected )
			{
				for ( int row = range.top(); row <= range.bottom(); ++row )
				{
					QModelIndex	  modelIndex = range.model()->index( row, 0, range.parent() );
					E_SYSTEM_ITEM item;
					Index		  index;
					SystemModel::unpack( modelIndex.internalId(), item, index );

					switch ( item )
					{
					case E_SYSTEM_ITEM::SYSTEM: deselected.addRange( system.getAtomRange() ); break;
					case E_SYSTEM_ITEM::CHAIN: deselected.addRange( system.getChainAtomRange( index ) ); break;
					case E_SYSTEM_ITEM::RESIDUE: deselected.addRange( system.getResidueAtomRange( index ) ); break;
					case E_SYSTEM_ITEM::ATOM: deselected.addValue( index ); break;
					default: break;
					}
				}
			}

			ACTION().execute<Action::Selection::SetSelected<E_SYSTEM_ITEM::ATOM>>( _system, deselected, false );
		}

		// Selected items.
		Core::Struct::IndexRangeList selected;
		if ( not p_selected.isEmpty() )
		{
			for ( const QItemSelectionRange & range : p_selected )
			{
				for ( int row = range.top(); row <= range.bottom(); ++row )
				{
					QModelIndex	  modelIndex = range.model()->index( row, 0, range.parent() );
					E_SYSTEM_ITEM item;
					Index		  index;
					SystemModel::unpack( modelIndex.internalId(), item, index );

					switch ( item )
					{
					case E_SYSTEM_ITEM::SYSTEM: selected.addRange( system.getAtomRange() ); break;
					case E_SYSTEM_ITEM::CHAIN: selected.addRange( system.getChainAtomRange( index ) ); break;
					case E_SYSTEM_ITEM::RESIDUE: selected.addRange( system.getResidueAtomRange( index ) ); break;
					case E_SYSTEM_ITEM::ATOM: selected.addValue( index ); break;
					default: break;
					}
				}
			}

			ACTION().execute<Action::Selection::SetSelected<E_SYSTEM_ITEM::ATOM>>( _system, selected, true );
		}
	}

	void SystemSelectionModel::refresh()
	{
		using namespace App;
		using namespace Core::Struct;

		auto &		 reg	   = REG();
		const auto & system	   = reg.get<Core::Struct::System>( _system );
		const auto & selection = reg.get<App::System::Selection>( _system );
		const auto & uid	   = reg.get<App::System::UID>( _system );
		const auto & model	   = *static_cast<const SystemModel *>( this->model() );

		QSignalBlocker blocker( this );
		QItemSelection qSelection;

		if ( Helper::System::isFullySelected<E_SYSTEM_ITEM::SYSTEM>( _system ) )
		{
			const QModelIndex index = model.makeIndex( 0, E_SYSTEM_ITEM::SYSTEM, 0 );
			qSelection.select( index, index );
		}
		else if ( Helper::System::isSelected<E_SYSTEM_ITEM::SYSTEM>( _system ) )
		{
			// Chains.
			for ( Index chain = 0; chain < system.getChainCount(); ++chain )
			{
				QString chainName = QString::fromStdString( system.getChainName( chain ) );
				if ( Helper::System::isFullySelected<E_SYSTEM_ITEM::CHAIN>( _system, chain ) )
				{
					const QModelIndex index = model.makeIndex( chain, E_SYSTEM_ITEM::CHAIN, chain );
					qSelection.select( index, index );
					continue;
				}
				else if ( Helper::System::isSelected<E_SYSTEM_ITEM::CHAIN>( _system, chain ) == false )
				{
					continue;
				}

				// Residues.
				for ( Index residue : system.getChainResidueRange( chain ) )
				{
					QString residueName = QString::fromStdString( system.getResidueName( residue ) );
					if ( Helper::System::isFullySelected<E_SYSTEM_ITEM::RESIDUE>( _system, residue ) )
					{
						const QModelIndex index = model.makeIndex(
							residue - system.getChainFirstResidue( chain ), E_SYSTEM_ITEM::RESIDUE, residue
						);
						qSelection.select( index, index );
						continue;
					}
					else if ( Helper::System::isSelected<E_SYSTEM_ITEM::RESIDUE>( _system, residue ) == false )
					{
						continue;
					}

					// Atoms.
					for ( Index atom : system.getResidueAtomRange( residue ) )
					{
						if ( Helper::System::isSelected<E_SYSTEM_ITEM::ATOM>( _system, atom ) )
						{
							const QModelIndex index = model.makeIndex(
								atom - system.getResidueFirstAtom( residue ), E_SYSTEM_ITEM::ATOM, atom
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

} // namespace VTX::UI::QT::Widget::Tree
