#include "app/helper/system.hpp"
#include "app/system/color.hpp"
#include <core/struct/system.hpp>

namespace VTX::App::Helper::System
{
	ECS::Entity getSystemByName( const std::string p_name ) noexcept
	{
		for ( auto it_entity : REG().view<Core::Struct::System>() )
		{
			auto & metadata = REG().get<Core::Struct::System>( it_entity );

			// For now we use the PDB code but the function aims to retrieve a system using what is visible to the user.
			if ( metadata.name == p_name )
				return it_entity;
		}

		return entt::null;
	}

	App::System::E_VISIBLE_STATE getVisibleState( const SystemItemView & p_system )
	{
		using namespace Core::Struct;
		using namespace App::System;

		const ECS::Entity ent = p_system.entity;

		if ( isFullyVisible<E_SYSTEM_ITEM::SYSTEM>( ent ) )
		{
			return App::System::E_VISIBLE_STATE::VISIBLE;
		}
		else
		{
			switch ( p_system.item )
			{
			case E_SYSTEM_ITEM::SYSTEM:
				return isVisible<E_SYSTEM_ITEM::SYSTEM>( ent ) ? E_VISIBLE_STATE::PARTIAL : E_VISIBLE_STATE::HIDDEN;
			case E_SYSTEM_ITEM::CHAIN:
				assert( p_system.index );
				return isFullyVisible<E_SYSTEM_ITEM::CHAIN>( ent, *p_system.index ) ? E_VISIBLE_STATE::VISIBLE
					   : isVisible<E_SYSTEM_ITEM::CHAIN>( ent, *p_system.index )	? E_VISIBLE_STATE::PARTIAL
																					: E_VISIBLE_STATE::HIDDEN;
			case E_SYSTEM_ITEM::RESIDUE:
				assert( p_system.index );
				return isFullyVisible<E_SYSTEM_ITEM::RESIDUE>( ent, *p_system.index ) ? E_VISIBLE_STATE::VISIBLE
					   : isVisible<E_SYSTEM_ITEM::RESIDUE>( ent, *p_system.index )	  ? E_VISIBLE_STATE::PARTIAL
																					  : E_VISIBLE_STATE::HIDDEN;
			case E_SYSTEM_ITEM::ATOM:
				assert( p_system.index );
				return isVisible<E_SYSTEM_ITEM::ATOM>( ent, *p_system.index ) ? E_VISIBLE_STATE::VISIBLE
																			  : E_VISIBLE_STATE::HIDDEN;
			default: break;
			}
		}

		return App::System::E_VISIBLE_STATE::HIDDEN;
	}

	std::optional<App::System::E_COLOR_SCHEME> getColorScheme( const SystemItemView & p_system )
	{
		using namespace Core::Struct;

		const auto & reg	= REG();
		const auto & system = reg.get<Core::Struct::System>( p_system.entity );
		const auto & color	= reg.get<App::System::Color>( p_system.entity );
		IndexRange	 atoms;

		switch ( p_system.item )
		{
		case E_SYSTEM_ITEM::SYSTEM: atoms = system.getAtomRange(); break;
		case E_SYSTEM_ITEM::CHAIN:
			assert( p_system.index );
			atoms = system.getChainAtomRange( *p_system.index );
			break;
		case E_SYSTEM_ITEM::RESIDUE:
			assert( p_system.index );
			atoms = system.getResidueAtomRange( *p_system.index );
			break;
		case E_SYSTEM_ITEM::ATOM:
			assert( p_system.index );
			atoms = IndexRange( *p_system.index );
			break;
		default: break;
		}

		for ( const auto & [ scheme, rangeList ] : color.colorSchemeAtoms )
		{
			if ( rangeList.contains( atoms ) )
			{
				return scheme;
			}
		}

		return std::nullopt;
	}

	bool isColorSchemeRoot( const SystemItemView & p_system )
	{
		using namespace Core::Struct;
		using namespace App::System;

		const ECS::Entity ent		   = p_system.entity;
		const auto &	  reg		   = REG();
		const auto &	  system	   = reg.get<Core::Struct::System>( ent );
		const auto &	  colorSchemes = reg.get<Color>( ent );

		for ( const auto & [ _, ranges ] : colorSchemes.colorSchemeAtoms )
		{
			// Check if indexes contained but not parent.
			switch ( p_system.item )
			{
			case E_SYSTEM_ITEM::SYSTEM:
				if ( ranges.count() == system.getAtomCount() )
				{
					return true;
				}
				break;
			case E_SYSTEM_ITEM::CHAIN:
			{
				assert( p_system.index );
				const IndexRange systemRange = system.getAtomRange();
				if ( ranges.contains( system.getChainAtomRange( *p_system.index ) )
					 && not ranges.contains( systemRange ) )
				{
					return true;
				}
			}
			break;
			case E_SYSTEM_ITEM::RESIDUE:
			{
				assert( p_system.index );
				const IndexRange chainRange
					= system.getChainAtomRange( system.getResidueChainIndex( *p_system.index ) );
				if ( ranges.contains( system.getResidueAtomRange( *p_system.index ) )
					 && not ranges.contains( chainRange ) )
				{
					return true;
				}
			}
			break;
			case E_SYSTEM_ITEM::ATOM:
			{
				assert( p_system.index );
				const IndexRange residueRange
					= system.getResidueAtomRange( system.getAtomResidueIndex( *p_system.index ) );
				if ( ranges.contains( *p_system.index ) && not ranges.contains( residueRange ) )
				{
					return true;
				}
				break;
			}
			default: break;
			}
		}

		return false;
	}

} // namespace VTX::App::Helper::System
