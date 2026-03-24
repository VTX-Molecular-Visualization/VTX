#include "ui/qt/model/system_selection_model.hpp"
#include "ui/qt/model/system_model.hpp"
#include <QGuiApplication>
#include <app/action/action_manager.hpp>
#include <app/action/selection.hpp>
#include <app/helper/system.hpp>
#include <app/services.hpp>
#include <app/system/uid.hpp>
#include <core/struct/topology.hpp>

namespace VTX::UI::QT::Model
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
		Util::ScopedChrono timer( "QT::Model::SystemSelectionModel::_selectionChanged" );

		using namespace App;
		using namespace Core::Struct;

		const SystemModel & model	 = *static_cast<const SystemModel *>( this->model() );
		const auto &		topology = REG().get<Core::Struct::Topology>( _system );

		// Unselect others.
		const Qt::KeyboardModifiers modifiers = QGuiApplication::keyboardModifiers();

		// Check Ctrl to clear others entities.
		const bool append = ( modifiers.testFlag( Qt::ControlModifier ) );

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
					case E_SYSTEM_ITEM::SYSTEM: deselected.addRange( topology.getAtomRange() ); break;
					case E_SYSTEM_ITEM::CHAIN: deselected.addRange( topology.getChainAtomRange( index ) ); break;
					case E_SYSTEM_ITEM::RESIDUE: deselected.addRange( topology.getResidueAtomRange( index ) ); break;
					case E_SYSTEM_ITEM::ATOM: deselected.addValue( index ); break;
					default: break;
					}
				}
			}

			ACTION().execute<Action::Selection::SetSelected<E_SYSTEM_ITEM::ATOM>>( _system, deselected, false, true );
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
					case E_SYSTEM_ITEM::SYSTEM: selected.addRange( topology.getAtomRange() ); break;
					case E_SYSTEM_ITEM::CHAIN: selected.addRange( topology.getChainAtomRange( index ) ); break;
					case E_SYSTEM_ITEM::RESIDUE: selected.addRange( topology.getResidueAtomRange( index ) ); break;
					case E_SYSTEM_ITEM::ATOM: selected.addValue( index ); break;
					default: break;
					}
				}
			}

			ACTION().execute<Action::Selection::SetSelected<E_SYSTEM_ITEM::ATOM>>( _system, selected, true, true );
		}
	}

	void SystemSelectionModel::refresh()
	{
		using namespace App;
		using namespace Core::Struct;

		auto &		 reg	   = REG();
		const auto & topology  = reg.get<Core::Struct::Topology>( _system );
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
			for ( Index chain = 0; chain < topology.getChainCount(); ++chain )
			{
				QString chainName = QString::fromStdString( topology.getChainName( chain ) );
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
				for ( Index residue : topology.getChainResidueRange( chain ) )
				{
					QString residueName = QString::fromStdString( topology.getResidueName( residue ) );
					if ( Helper::System::isFullySelected<E_SYSTEM_ITEM::RESIDUE>( _system, residue ) )
					{
						const QModelIndex index = model.makeIndex(
							residue - topology.getChainFirstResidue( chain ), E_SYSTEM_ITEM::RESIDUE, residue
						);
						qSelection.select( index, index );
						continue;
					}
					else if ( Helper::System::isSelected<E_SYSTEM_ITEM::RESIDUE>( _system, residue ) == false )
					{
						continue;
					}

					// Atoms.
					for ( Index atom : topology.getResidueAtomRange( residue ) )
					{
						if ( Helper::System::isSelected<E_SYSTEM_ITEM::ATOM>( _system, atom ) )
						{
							const QModelIndex index = model.makeIndex(
								atom - topology.getResidueFirstAtom( residue ), E_SYSTEM_ITEM::ATOM, atom
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

} // namespace VTX::UI::QT::Model
