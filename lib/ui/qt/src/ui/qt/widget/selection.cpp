#include "ui/qt/widget/selection.hpp"
#include "ui/qt/services.hpp"
#include <app/helper/system.hpp>
#include <app/services.hpp>
#include <app/system/metadata.hpp>
#include <app/system/selection.hpp>
#include <core/struct/system.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::Widget
{
	Selection::Selection( QWidget * const p_parent ) : BaseWidget( p_parent )
	{
		// Disable selection.
		this->setSelectionMode( QAbstractItemView::NoSelection );

		// Refresh widget when selection changed.
		App::HUB().connect<App::Events::SelectionChange, &Selection::_onSelectionChange>( this );
	}

	void Selection::_onSelectionChange( const App::Events::SelectionChange & p_e )
	{
		using namespace App;
		using namespace App::Scene;

		auto &	   reg		= REG();
		const auto entities = reg.view<App::System::Selection>();

		// Delete all items.
		this->clear();

		// Add selected items.
		for ( auto entity : entities )
		{
			const auto & system	   = reg.get<Core::Struct::System>( entity );
			const auto & selection = reg.get<App::System::Selection>( entity );

			QString name = QString::fromStdString( system.name );
			if ( Helper::System::isFullySelected<E_ITEM::SYSTEM>( entity ) )
			{
				addItem( name );
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
					addItem( name + "/" + chainName );
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
						addItem( name + "/" + chainName + "/" + residueName );
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
							addItem(
								name + "/" + chainName + "/" + residueName + "/"
								+ QString::fromStdString( system.getAtomName( atom ) )
							);
						}
					}
				}
			}
		}
	}
} // namespace VTX::UI::QT::Widget
