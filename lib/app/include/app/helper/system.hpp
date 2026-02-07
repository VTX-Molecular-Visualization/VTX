#ifndef __VTX_APP_HELPER_VISIBILITY__
#define __VTX_APP_HELPER_VISIBILITY__

#include "app/ecs.hpp"
#include "app/scene/tag_root.hpp"
#include "app/system/color.hpp"
#include "app/system/selection.hpp"
#include "app/system/visibility.hpp"
#include <core/struct/system.hpp>

/**
 * @brief Request data from entity, READ-ONLY.
 */
namespace VTX::App::Helper::System
{
	/**
	 * @brief Check if an item is visible.
	 */
	template<Core::Struct::E_SYSTEM_ITEM ITEM>
	bool isVisible( const ECS::Entity p_ent, const std::optional<Index> p_index = std::nullopt )
	{
		using namespace Core::Struct;

		const auto & system		= REG().get<Core::Struct::System>( p_ent );
		const auto & visibility = REG().get<App::System::Visibility>( p_ent );

		if constexpr ( ITEM == E_SYSTEM_ITEM::SYSTEM )
		{
			return not visibility.atoms.isEmpty();
		}

		assert( p_index );

		if constexpr ( ITEM == E_SYSTEM_ITEM::CHAIN )
		{
			return visibility.atoms.intersects( system.getChainAtomRange( *p_index ) );
		}
		else if constexpr ( ITEM == E_SYSTEM_ITEM::RESIDUE )
		{
			return visibility.atoms.intersects( system.getResidueAtomRange( *p_index ) );
		}
		else if constexpr ( ITEM == E_SYSTEM_ITEM::ATOM )
		{
			return visibility.atoms.contains( *p_index );
		}
		else
		{
			return false;
		}
	}

	/**
	 * @brief Check if an item is fully visible.
	 */
	template<Core::Struct::E_SYSTEM_ITEM ITEM>
	bool isFullyVisible( const ECS::Entity p_ent, const std::optional<Index> p_index = std::nullopt )
	{
		using namespace Core::Struct;

		const auto & system		= REG().get<Core::Struct::System>( p_ent );
		const auto & visibility = REG().get<App::System::Visibility>( p_ent );

		if constexpr ( ITEM == E_SYSTEM_ITEM::SYSTEM )
		{
			return visibility.atoms.count() == system.getAtomCount();
		}

		assert( p_index );

		if constexpr ( ITEM == E_SYSTEM_ITEM::CHAIN )
		{
			return visibility.atoms.contains( system.getChainAtomRange( *p_index ) );
		}
		else if constexpr ( ITEM == E_SYSTEM_ITEM::RESIDUE )
		{
			return visibility.atoms.contains( system.getResidueAtomRange( *p_index ) );
		}
		else if constexpr ( ITEM == E_SYSTEM_ITEM::ATOM )
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
	template<Core::Struct::E_SYSTEM_ITEM ITEM>
	bool isSelected( const ECS::Entity p_ent, const std::optional<Index> p_index = std::nullopt )
	{
		using namespace Core::Struct;

		const auto & system	   = REG().get<Core::Struct::System>( p_ent );
		const auto & selection = REG().get<App::System::Selection>( p_ent );

		if constexpr ( ITEM == E_SYSTEM_ITEM::SYSTEM )
		{
			return not selection.atoms.isEmpty();
		}

		assert( p_index );

		if constexpr ( ITEM == E_SYSTEM_ITEM::CHAIN )
		{
			return selection.atoms.intersects( system.getChainAtomRange( *p_index ) );
		}
		else if constexpr ( ITEM == E_SYSTEM_ITEM::RESIDUE )
		{
			return selection.atoms.intersects( system.getResidueAtomRange( *p_index ) );
		}
		else if constexpr ( ITEM == E_SYSTEM_ITEM::ATOM )
		{
			return selection.atoms.contains( *p_index );
		}
		else
		{
			return false;
		}
	}

	/**
	 * @brief Check if an item is fully selected.
	 */
	template<Core::Struct::E_SYSTEM_ITEM ITEM>
	bool isFullySelected( const ECS::Entity p_ent, const std::optional<Index> p_index = std::nullopt )
	{
		using namespace Core::Struct;

		const auto & system	   = REG().get<Core::Struct::System>( p_ent );
		const auto & selection = REG().get<App::System::Selection>( p_ent );

		if constexpr ( ITEM == E_SYSTEM_ITEM::SYSTEM )
		{
			return selection.atoms.count() == system.getAtomCount();
		}

		assert( p_index );

		if constexpr ( ITEM == E_SYSTEM_ITEM::CHAIN )
		{
			return selection.atoms.contains( system.getChainAtomRange( *p_index ) );
		}
		else if constexpr ( ITEM == E_SYSTEM_ITEM::RESIDUE )
		{
			return selection.atoms.contains( system.getResidueAtomRange( *p_index ) );
		}
		else if constexpr ( ITEM == E_SYSTEM_ITEM::ATOM )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/**
	 * @brief Struct that describes a system item.
	 */
	struct SystemItemView
	{
		ECS::Entity					entity;
		Core::Struct::E_SYSTEM_ITEM item;
		std::optional<Index>		index = std::nullopt;
	};

	/**
	 * @brief Get the visibility state of an item.
	 */
	App::System::E_VISIBLE_STATE getVisibleState( const SystemItemView & );

	/**
	 * @brief Get the color scheme of an item (nothing if multiples).
	 */
	std::optional<App::System::E_COLOR_SCHEME> getColorScheme( const SystemItemView & );

	/**
	 * @brief Check if an item is a color scheme root.
	 */
	bool isColorSchemeRoot( const SystemItemView & );

	/**
	 * @brief Get the representation of an item (nothing if multiples).
	 */
	std::optional<ECS::Entity> getRepresentation( const SystemItemView & );

	/**
	 * @brief Check if an item is a representation root.
	 */
	bool isRepresentationRoot( const SystemItemView & );

	/**
	 * @brief Retrieve a system with the corresponding name. Initially meant to be used for scripting purposes.
	 * @param
	 * @return
	 */
	ECS::Entity getSystemByName( const std::string ) noexcept;

} // namespace VTX::App::Helper::System

#endif
