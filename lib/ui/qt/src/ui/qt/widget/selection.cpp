#include "ui/qt/widget/selection.hpp"
#include "ui/qt/services.hpp"
#include <app/helper/system.hpp>
#include <app/services.hpp>
#include <app/system/metadata.hpp>
#include <app/system/selection.hpp>
#include <core/struct/topology.hpp>
#include <util/event_hub.hpp>

namespace
{
	// TODO: use and make configurable?
	constexpr uint MAX_DISPLAYED = 50;
} // namespace

namespace VTX::UI::QT::Widget
{
	Selection::Selection( QWidget * const p_parent ) : BaseWidget( p_parent )
	{
		// Disable selection.
		this->setSelectionMode( QAbstractItemView::NoSelection );

		// Refresh widget when selection changed.
		App::REG().on_update<App::System::Selection>().connect<&Selection::_onUpdateSelection>( this );
	}

	// TODO: optimize and factorize.
	void Selection::_onUpdateSelection( App::ECS::Registry &, App::ECS::Entity p_e )
	{
		Util::ScopedChrono timer( "QT::Widget::Selection::_onUpdateSelection" );

		using namespace App;
		using namespace Core::Struct;

		auto &	   reg		= REG();
		const auto entities = reg.view<App::System::Selection>();

		// Delete all items.
		this->clear();

		// Add selected items.
		for ( auto entity : entities )
		{
			const auto & topology  = reg.get<Core::Struct::Topology>( entity );
			const auto & selection = reg.get<App::System::Selection>( entity );

			QString name = QString::fromStdString( topology.name );
			if ( Helper::System::isFullySelected<E_SYSTEM_ITEM::SYSTEM>( entity ) )
			{
				addItem( name );
				continue;
			}
			else if ( Helper::System::isSelected<E_SYSTEM_ITEM::SYSTEM>( entity ) == false )
			{
				continue;
			}

			// Chains.
			for ( Index chain = 0; chain < topology.getChainCount(); ++chain )
			{
				QString chainName = QString::fromStdString( topology.getChainName( chain ) );
				if ( Helper::System::isFullySelected<E_SYSTEM_ITEM::CHAIN>( entity, chain ) )
				{
					addItem( name + "/" + chainName );
					continue;
				}
				else if ( Helper::System::isSelected<E_SYSTEM_ITEM::CHAIN>( entity, chain ) == false )
				{
					continue;
				}

				// Residues.
				for ( Index residue : topology.getChainResidueRange( chain ) )
				{
					QString residueName = QString::fromStdString( topology.getResidueName( residue ) );
					if ( Helper::System::isFullySelected<E_SYSTEM_ITEM::RESIDUE>( entity, residue ) )
					{
						addItem( name + "/" + chainName + "/" + residueName );
						continue;
					}
					else if ( Helper::System::isSelected<E_SYSTEM_ITEM::RESIDUE>( entity, residue ) == false )
					{
						continue;
					}

					// Atoms.
					for ( Index atom : topology.getResidueAtomRange( residue ) )
					{
						if ( Helper::System::isSelected<E_SYSTEM_ITEM::ATOM>( entity, atom ) )
						{
							addItem(
								name + "/" + chainName + "/" + residueName + "/"
								+ QString::fromStdString( topology.getAtomName( atom ) )
							);
						}
					}
				}
			}
		}
	}
} // namespace VTX::UI::QT::Widget
