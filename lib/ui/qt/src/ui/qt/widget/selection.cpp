#include "ui/qt/widget/selection.hpp"
#include "ui/qt/model.hpp"
#include "ui/qt/selection_model.hpp"
#include "ui/qt/services.hpp"
#include <app/helper/scene.hpp>
#include <app/system/metadata.hpp>

namespace VTX::UI::QT::Widget
{
	Selection::Selection( QWidget * const p_parent ) : BaseWidget( p_parent )
	{
		// Disable selection.
		this->setSelectionMode( QAbstractItemView::NoSelection );
	}

	void Selection::refreshSelection()
	{
		using namespace App::Scene;

		auto &				  model			 = MODEL();
		auto &				  selectionModel = SELECTION();
		const QModelIndexList rows			 = selectionModel.selectedRows();

		// Delete all items.
		this->clear();

		// Add selected items.
		for ( const QModelIndex & index : rows )
		{
			if ( not index.isValid() )
			{
				this->addItem( "<invalid>" );
				continue;
			}

			// Get data.
			const E_ITEM  item		  = index.data( Model::ItemRole ).value<E_ITEM>();
			const RootUID globalIndex = index.data( Model::RootRole ).value<RootUID>();
			const Index	  localIndex  = index.data( Model::LocalRole ).value<Index>();

			// Get components.
			// TODO: not efficient.
			App::ECS::Entity ent	  = App::Helper::Scene::findSystemByRootUID( globalIndex ).value();
			auto &			 system	  = App::REG().get<Core::Struct::System>( ent );
			auto &			 metadata = App::REG().get<App::System::Metadata>( ent );

			QString display;

			// Add pdb id.
			display += QString::fromStdString( metadata.pdbIDCode );

			std::optional<QString> chainName;
			std::optional<QString> residueName;
			std::optional<QString> atomName;

			switch ( item )
			{
			case E_ITEM::SYSTEM: break;
			case E_ITEM::CHAIN: chainName = QString::fromStdString( system.chainNames[ localIndex ] ); break;
			case E_ITEM::RESIDUE:
				residueName = QString::fromStdString( system.residueNames[ localIndex ] );
				chainName	= QString::fromStdString( system.chainNames[ system.residueChainIndexes[ localIndex ] ] );
				break;
			case E_ITEM::ATOM:
				atomName	= QString::fromStdString( system.atomNames[ localIndex ] );
				residueName = QString::fromStdString( system.residueNames[ system.atomResidueIndexes[ localIndex ] ] );
				chainName	= QString::fromStdString(
					  system.chainNames[ system.residueChainIndexes[ system.atomResidueIndexes[ localIndex ] ] ]
				  );
				break;
			default: break;
			}

			if ( chainName.has_value() )
			{
				display += "/" + chainName.value();
			}
			if ( residueName.has_value() )
			{
				display += "/" + residueName.value();
			}
			if ( atomName.has_value() )
			{
				display += "/" + atomName.value();
			}

			this->addItem( display );
		}
	}

} // namespace VTX::UI::QT::Widget
