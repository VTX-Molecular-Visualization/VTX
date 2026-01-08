#ifndef __VTX_APP_HELPER_VISIBILITY__
#define __VTX_APP_HELPER_VISIBILITY__

#include "app/ecs.hpp"
#include "app/scene/tag_root.hpp"
#include "app/system/selection.hpp"
#include "app/system/visibility.hpp"
#include <core/struct/system.hpp>

namespace VTX::App::Helper::System
{
	/**
	 * @brief Check if an item is visible.
	 */
	template<Scene::E_ITEM ITEM>
	bool isVisible( const ECS::Entity p_ent, const Index p_index = INVALID_INDEX )
	{
		const auto & system		= REG().get<Core::Struct::System>( p_ent );
		auto &		 visibility = REG().get<App::System::Visibility>( p_ent );

		if constexpr ( ITEM == App::Scene::E_ITEM::SYSTEM )
		{
			return not visibility.atoms.isEmpty();
		}

		assert( p_index != INVALID_INDEX );

		if constexpr ( ITEM == App::Scene::E_ITEM::CHAIN )
		{
			return visibility.atoms.intersects( system.getChainAtomRange( p_index ) );
		}
		else if constexpr ( ITEM == App::Scene::E_ITEM::RESIDUE )
		{
			return visibility.atoms.intersects( system.getResidueAtomRange( p_index ) );
		}
		else if constexpr ( ITEM == App::Scene::E_ITEM::ATOM )
		{
			return visibility.atoms.contains( p_index );
		}
		else
		{
			return false;
		}
	}

	/**
	 * @brief Check if an item is fully visible.
	 */
	template<Scene::E_ITEM ITEM>
	bool isFullyVisible( const ECS::Entity p_ent, const Index p_index = INVALID_INDEX )
	{
		const auto & system		= REG().get<Core::Struct::System>( p_ent );
		auto &		 visibility = REG().get<App::System::Visibility>( p_ent );

		if constexpr ( ITEM == App::Scene::E_ITEM::SYSTEM )
		{
			return visibility.atoms.count() == system.getAtomCount();
		}

		assert( p_index != INVALID_INDEX );

		if constexpr ( ITEM == App::Scene::E_ITEM::CHAIN )
		{
			return visibility.atoms.contains( system.getChainAtomRange( p_index ) );
		}
		else if constexpr ( ITEM == App::Scene::E_ITEM::RESIDUE )
		{
			return visibility.atoms.contains( system.getResidueAtomRange( p_index ) );
		}
		else if constexpr ( ITEM == App::Scene::E_ITEM::ATOM )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/**
	 * @brief Check if an item is selected.
	 */
	template<Scene::E_ITEM ITEM>
	bool isSelected( const ECS::Entity p_ent, const Index p_index = INVALID_INDEX )
	{
		const auto & system	   = REG().get<Core::Struct::System>( p_ent );
		auto &		 selection = REG().get<App::System::Selection>( p_ent );

		if constexpr ( ITEM == App::Scene::E_ITEM::SYSTEM )
		{
			return not selection.atoms.isEmpty();
		}

		assert( p_index != INVALID_INDEX );

		if constexpr ( ITEM == App::Scene::E_ITEM::CHAIN )
		{
			return selection.atoms.intersects( system.getChainAtomRange( p_index ) );
		}
		else if constexpr ( ITEM == App::Scene::E_ITEM::RESIDUE )
		{
			return selection.atoms.intersects( system.getResidueAtomRange( p_index ) );
		}
		else if constexpr ( ITEM == App::Scene::E_ITEM::ATOM )
		{
			return selection.atoms.contains( p_index );
		}
		else
		{
			return false;
		}
	}

	/**
	 * @brief Check if an item is fully selected.
	 */
	template<Scene::E_ITEM ITEM>
	bool isFullySelected( const ECS::Entity p_ent, const Index p_index = INVALID_INDEX )
	{
		const auto & system	   = REG().get<Core::Struct::System>( p_ent );
		auto &		 selection = REG().get<App::System::Selection>( p_ent );

		if constexpr ( ITEM == App::Scene::E_ITEM::SYSTEM )
		{
			return selection.atoms.count() == system.getAtomCount();
		}

		assert( p_index != INVALID_INDEX );

		if constexpr ( ITEM == App::Scene::E_ITEM::CHAIN )
		{
			return selection.atoms.contains( system.getChainAtomRange( p_index ) );
		}
		else if constexpr ( ITEM == App::Scene::E_ITEM::RESIDUE )
		{
			return selection.atoms.contains( system.getResidueAtomRange( p_index ) );
		}
		else if constexpr ( ITEM == App::Scene::E_ITEM::ATOM )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

} // namespace VTX::App::Helper::System

#endif
