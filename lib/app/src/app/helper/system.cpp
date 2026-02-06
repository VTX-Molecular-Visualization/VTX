#include "app/helper/system.hpp"

namespace VTX::App::Helper::System
{

	App::System::E_VISIBLE_STATE getVisibleState(
		const ECS::Entity				  p_ent,
		const Core::Struct::E_SYSTEM_ITEM p_item,
		const Index						  p_index
	)
	{
		using namespace Core::Struct;
		using namespace App::System;

		if ( isFullyVisible<E_SYSTEM_ITEM::SYSTEM>( p_ent ) )
		{
			return App::System::E_VISIBLE_STATE::VISIBLE;
		}
		else
		{
			switch ( p_item )
			{
			case E_SYSTEM_ITEM::SYSTEM:
				return isVisible<E_SYSTEM_ITEM::SYSTEM>( p_ent ) ? E_VISIBLE_STATE::PARTIAL : E_VISIBLE_STATE::HIDDEN;
			case E_SYSTEM_ITEM::CHAIN:
				return isFullyVisible<E_SYSTEM_ITEM::CHAIN>( p_ent, p_index ) ? E_VISIBLE_STATE::VISIBLE
					   : isVisible<E_SYSTEM_ITEM::CHAIN>( p_ent, p_index )	  ? E_VISIBLE_STATE::PARTIAL
																			  : E_VISIBLE_STATE::HIDDEN;
			case E_SYSTEM_ITEM::RESIDUE:
				return isFullyVisible<E_SYSTEM_ITEM::RESIDUE>( p_ent, p_index ) ? E_VISIBLE_STATE::VISIBLE
					   : isVisible<E_SYSTEM_ITEM::RESIDUE>( p_ent, p_index )	? E_VISIBLE_STATE::PARTIAL
																				: E_VISIBLE_STATE::HIDDEN;
			case E_SYSTEM_ITEM::ATOM:
				return isVisible<E_SYSTEM_ITEM::ATOM>( p_ent, p_index ) ? E_VISIBLE_STATE::VISIBLE
																		: E_VISIBLE_STATE::HIDDEN;
			default: break;
			}
		}

		return App::System::E_VISIBLE_STATE::HIDDEN;
	}

} // namespace VTX::App::Helper::System
