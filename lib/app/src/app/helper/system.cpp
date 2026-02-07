#include "app/helper/system.hpp"
#include "app/system/color.hpp"

namespace VTX::App::Helper::System
{

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
		//
		return std::nullopt;
	}

	bool isColorSchemeRoot( const SystemItemView & p_system )
	{
		using namespace Core::Struct;
		using namespace App::System;

		const ECS::Entity ent = p_system.entity;

		auto & system		= REG().get<Core::Struct::System>( ent );
		auto & colorSchemes = REG().get<Color>( ent );

		for ( const auto & [ _, ranges ] : colorSchemes.colorSchemeAtoms )
		{
			switch ( p_system.item )
			{
			case E_SYSTEM_ITEM::SYSTEM:
				if ( ranges.count() == system.getAtomCount() )
				{
					return true;
				}
				break;
			case E_SYSTEM_ITEM::CHAIN:
				assert( p_system.index );
				if ( ranges.equals( system.getChainAtomRange( *p_system.index ) ) )
				{
					return true;
				}
				break;
			case E_SYSTEM_ITEM::RESIDUE:
				assert( p_system.index );
				if ( ranges.equals( system.getResidueAtomRange( *p_system.index ) ) )
				{
					return true;
				}
				break;
			case E_SYSTEM_ITEM::ATOM:
				assert( p_system.index );
				if ( ranges.contains( *p_system.index ) )
				{
					return true;
				}
				break;
			default: break;
			}
		}

		return false;
	}

} // namespace VTX::App::Helper::System
