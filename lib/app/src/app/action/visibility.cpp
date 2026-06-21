#include <app/ecs.hpp>
#include <app/system/visibility.hpp>
//
#include <app/action/visibility.hpp>

namespace VTX::App::Action::Visibility
{

	void patchVisibility( const Entity p_ent, Util::Math::BitSet p_atoms )
	{
		auto &		 reg		= REG();
		const auto & visibility = reg.get<System::Visibility>( p_ent );
		if ( visibility.atoms == p_atoms )
		{
			return;
		}

		reg.patch<System::Visibility>(
			p_ent, [ &p_atoms ]( System::Visibility & p_visibility ) { p_visibility.atoms = std::move( p_atoms ); }
		);
	}

	void HideEverything::execute()
	{
		for ( auto entity : App::REG().view<System::Visibility>() )
		{
			App::REG().patch<System::Visibility>( entity, []( System::Visibility & p_vis ) { p_vis.atoms.clear(); } );
		}
	}

	void SetVisibleSelected::execute( const bool p_visible )
	{
		REG().view<System::Selection, System::Visibility>().each(
			[ p_visible ](
				const Entity p_ent, const System::Selection & p_selection, const System::Visibility & p_visibility
			)
			{
				if ( p_selection.atoms.any() )
				{
					Util::Math::BitSet current = p_visibility.atoms;
					if ( p_visible )
					{
						current.mergeInPlace( p_selection.atoms );
					}
					else
					{
						current.subtractInPlace( p_selection.atoms );
					}

					patchVisibility( p_ent, std::move( current ) );
				}
			}
		);
	}

	void SetVisibleItem::execute(
		const Entity						 p_ent,
		const Core::Struct::E_SYSTEM_ITEM	 p_item,
		const Core::Struct::IndexRangeList & p_ranges,
		const bool							 p_visible
	)
	{
		switch ( p_item )
		{
		case Core::Struct::E_SYSTEM_ITEM::SYSTEM:
			SetVisible<Core::Struct::E_SYSTEM_ITEM::SYSTEM>().execute( p_ent, p_ranges, p_visible );
			break;
		case Core::Struct::E_SYSTEM_ITEM::CATEGORY:
			SetVisible<Core::Struct::E_SYSTEM_ITEM::CATEGORY>().execute( p_ent, p_ranges, p_visible );
			break;
		case Core::Struct::E_SYSTEM_ITEM::CHAIN:
			SetVisible<Core::Struct::E_SYSTEM_ITEM::CHAIN>().execute( p_ent, p_ranges, p_visible );
			break;
		case Core::Struct::E_SYSTEM_ITEM::RESIDUE:
			SetVisible<Core::Struct::E_SYSTEM_ITEM::RESIDUE>().execute( p_ent, p_ranges, p_visible );
			break;
		case Core::Struct::E_SYSTEM_ITEM::ATOM:
			SetVisible<Core::Struct::E_SYSTEM_ITEM::ATOM>().execute( p_ent, p_ranges, p_visible );
			break;
		default: assert( false && "Unhandled E_SYSTEM_ITEM type in SetVisibleItem action." ); break;
		}
	}
} // namespace VTX::App::Action::Visibility
