#include "ui/qt/menu/selection.hpp"
#include "ui/qt/model.hpp"
#include "ui/qt/selection_model.hpp"
#include "ui/qt/services.hpp"
#include <QModelIndexList>
#include <app/action/scene.hpp>
#include <ranges>

namespace VTX::UI::QT::Menu
{
	Selection::Selection( QWidget * p_parent ) : BaseWidget( p_parent )
	{
		setTitle( "Selection" );

		auto & selection = SELECTION();

		// Refresh menu when opened.
		connect( this, &QMenu::aboutToShow, this, &Selection::_refresh );

		// Renable/disable menu based on selection.
		connect(
			&selection,
			&QItemSelectionModel::selectionChanged,
			this,
			[ this, &selection ]( const QItemSelection &, const QItemSelection & )
			{ this->setEnabled( selection.hasSelection() ); }
		);

		setEnabled( selection.hasSelection() );
	}

	void Selection::_refresh()
	{
		using namespace App::Scene;

		// Clear previous actions.
		clear();

		/*
		addAction<Action::Selection::SetGranularitySystem>();
		addAction<Action::Selection::SetGranularityChain>();
		addAction<Action::Selection::SetGranularityResidue>();
		addAction<Action::Selection::SetGranularityAtom>();
		*/

		// TODO: parameters?
		auto & selectionModel = SELECTION();
		auto & model		  = MODEL();

		const size_t			  count = size_t( E_ITEM::COUNT );
		std::array<size_t, count> rowsPerItem;
		rowsPerItem.fill( 0 );
		std::vector<SystemUID> systemGlobalIndexes;

		const QModelIndex	  clickedRow = selectionModel.currentIndex(); // TODO: not valid in menubar context.
		const QModelIndexList rows		 = selectionModel.selectedRows();
		for ( const QModelIndex & index : rows )
		{
			if ( not index.isValid() )
			{
				continue;
			}

			// Decode index to get entity.
			E_ITEM	item;
			SystemUID globalIndex;
			Index	localIndex;
			Model::unpack( index.internalId(), item, globalIndex, localIndex );

			rowsPerItem[ size_t( item ) ] += 1;

			if ( item == E_ITEM::SYSTEM )
			{
				systemGlobalIndexes.push_back( globalIndex );
			}
		}

		const size_t systemRows	 = rowsPerItem[ size_t( E_ITEM::SYSTEM ) ];
		const size_t chainRows	 = rowsPerItem[ size_t( E_ITEM::CHAIN ) ];
		const size_t residueRows = rowsPerItem[ size_t( E_ITEM::RESIDUE ) ];
		const size_t atomRows	 = rowsPerItem[ size_t( E_ITEM::ATOM ) ];

		if ( systemRows > 0 )
		{
			addSection(
				QString( "%1 system%2" ).arg( QString::number( systemRows ) ).arg( systemRows > 1 ? "s" : "" )
			);
			auto * const action = addAction<Action::System::Delete>();
			connect(
				action,
				&QAction::triggered,
				[ &model, systemGlobalIndexes ]()
				{
					//					auto & mapGlobalId = model.getMapRows();
					//					for ( const auto & systemGlobalIndex : systemGlobalIndexes )
					//					{
					//						App::ACTION().execute<App::Action::Scene::DeleteSystem>(
					//							mapGlobalId.at( systemGlobalIndex )->entity
					//						);
					//					}
				}
			);
		}

		if ( chainRows > 0 )
		{
			addSection( QString( "%1 chain%2" ).arg( QString::number( chainRows ) ).arg( chainRows > 1 ? "s" : "" ) );
		}

		if ( residueRows > 0 )
		{
			addSection(
				QString( "%1 residue%2" ).arg( QString::number( residueRows ) ).arg( residueRows > 1 ? "s" : "" )
			);
		}

		if ( atomRows > 0 )
		{
			addSection( QString( "%1 atom%2" ).arg( QString::number( atomRows ) ).arg( atomRows > 1 ? "s" : "" ) );
		}
	}
} // namespace VTX::UI::QT::Menu
