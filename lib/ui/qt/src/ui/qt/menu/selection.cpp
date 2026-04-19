#include "ui/qt/menu/selection.hpp"
#include <app/action/scene.hpp>
#include <app/helper/system.hpp>
#include <app/system/metadata.hpp>
#include <app/system/selection.hpp>
#include <core/struct/topology.hpp>

namespace VTX::UI::QT::Menu
{
	Selection::Selection( QWidget * p_parent ) : BaseWidget( p_parent )
	{
		setTitle( "Selection" );

		// Refresh menu when opened.
		connect( this, &QMenu::aboutToShow, this, &Selection::_refresh );
	}

	void Selection::_refresh()
	{
		using namespace App;
		using namespace Core::Struct;

		// Clear previous actions.
		clear();

		// TODO: Switch on E_SELECTION_GROUP.
		// auto & selectionModel = SELECTION();
		//
		// But for now, just check system selection from App.
		auto &	   reg		= REG();
		const auto entities = reg.view<App::System::Selection>();

		Index countSystem  = 0;
		Index countChain   = 0;
		Index countResidue = 0;
		Index countAtom	   = 0;

		for ( auto entity : entities )
		{
			const auto & topology  = reg.get<Core::Struct::Topology>( entity );
			const auto & selection = reg.get<App::System::Selection>( entity );
			const auto & metadata  = reg.get<App::System::Metadata>( entity );

			QString name = QString::fromStdString( topology.name );

			const auto systemState = Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::SYSTEM } );
			if ( systemState == App::System::E_SELECTION_STATE::FULL )
			{
				countSystem++;
				countChain += topology.getChainCount();
				countResidue += topology.getResidueCount();
				countAtom += topology.getAtomCount();

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
					countChain++;
					countResidue += topology.getChainResidueCount( chain );
					countAtom += topology.getChainAtomCount( chain );

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
						countResidue++;
						countAtom += topology.getResidueAtomCount( residue );

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
							countAtom++;
						}
					}
				}
			}
		}

		if ( countSystem > 0 )
		{
			addSection(
				QString( "%1 system%2" ).arg( QString::number( countSystem ) ).arg( countSystem > 1 ? "s" : "" )
			);
			auto * const action = addAction<Action::System::Delete>();
		}

		if ( countChain > 0 )
		{
			addSection( QString( "%1 chain%2" ).arg( QString::number( countChain ) ).arg( countChain > 1 ? "s" : "" ) );
			auto * const action = addAction<Action::System::Delete>();
		}

		if ( countResidue > 0 )
		{
			addSection(
				QString( "%1 residue%2" ).arg( QString::number( countResidue ) ).arg( countResidue > 1 ? "s" : "" )
			);
		}

		if ( countAtom > 0 )
		{
			addSection( QString( "%1 atom%2" ).arg( QString::number( countAtom ) ).arg( countAtom > 1 ? "s" : "" ) );
		}
	}
} // namespace VTX::UI::QT::Menu
