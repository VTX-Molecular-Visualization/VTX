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
		refresh();
	}

	// TODO: optimize and factorize.
	void Selection::refresh()
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

			QString	   name		   = QString::fromStdString( topology.name );
			const auto systemState = Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::SYSTEM } );
			if ( systemState == App::System::E_SELECTION_STATE::FULL )
			{
				addItem( name );
				continue;
			}
			else if ( systemState == App::System::E_SELECTION_STATE::NONE )
			{
				continue;
			}

			// Chains.
			for ( Index chain = 0; chain < topology.getChainCount(); ++chain )
			{
				QString	   chainName  = QString::fromStdString( topology.getChainName( chain ) );
				const auto chainState = Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::CHAIN, chain } );
				if ( chainState == App::System::E_SELECTION_STATE::FULL )
				{
					addItem( name + "/" + chainName );
					continue;
				}
				else if ( chainState == App::System::E_SELECTION_STATE::NONE )
				{
					continue;
				}

				// Residues.
				for ( Index residue : topology.getChainResidueRange( chain ) )
				{
					QString	   residueName = QString::fromStdString( topology.getResidueName( residue ) );
					const auto residueState
						= Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::RESIDUE, residue } );
					if ( residueState == App::System::E_SELECTION_STATE::FULL )
					{
						addItem( name + "/" + chainName + "/" + residueName );
						continue;
					}
					else if ( residueState == App::System::E_SELECTION_STATE::NONE )
					{
						continue;
					}

					// Atoms.
					for ( Index atom : topology.getResidueAtomRange( residue ) )
					{
						if ( Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::ATOM, atom } )
							 == App::System::E_SELECTION_STATE::FULL )
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
