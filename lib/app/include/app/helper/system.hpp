#ifndef __VTX_APP_HELPER_SYSTEM__
#define __VTX_APP_HELPER_SYSTEM__

#include "app/ecs.hpp"
#include "app/scene/tag_root.hpp"
#include "app/system/color.hpp"
#include "app/system/selection.hpp"
#include "app/system/visibility.hpp"
#include <core/struct/topology.hpp>
#include <util/type_traits.hpp>

/**
 * @brief Request data from entity, READ-ONLY.
 */
namespace VTX::App::Helper::System
{
	/**
	 * @brief Get the atom range of an item.
	 */
	template<Core::Struct::E_SYSTEM_ITEM ITEM>
	Core::Struct::IndexRange getAtomRange( const ECS::Entity p_ent, const std::optional<Index> p_index = std::nullopt )
	{
		using namespace Core::Struct;

		const auto & topology = REG().get<Core::Struct::Topology>( p_ent );

		if constexpr ( ITEM == E_SYSTEM_ITEM::SYSTEM )
		{
			return topology.getAtomRange();
		}

		assert( p_index );

		if constexpr ( ITEM == E_SYSTEM_ITEM::CHAIN )
		{
			return topology.getChainAtomRange( *p_index );
		}
		else if constexpr ( ITEM == E_SYSTEM_ITEM::RESIDUE )
		{
			return topology.getResidueAtomRange( *p_index );
		}
		else if constexpr ( ITEM == E_SYSTEM_ITEM::ATOM )
		{
			return IndexRange { *p_index };
		}
		else
		{
			static_assert( always_false_v<ITEM>, "Unhandled E_ITEM type in getItemAtomRange()." );
		}
	}

	/**
	 * @brief Get the atom range list of an item.
	 */
	template<Core::Struct::E_SYSTEM_ITEM ITEM>
	Core::Struct::IndexRangeList getAtomRangeList(
		const ECS::Entity					 p_ent,
		const Core::Struct::IndexRangeList & p_ranges = {}
	)
	{
		using namespace Core::Struct;

		const auto &   topology = REG().get<Core::Struct::Topology>( p_ent );
		IndexRangeList atoms;

		if constexpr ( ITEM == E_SYSTEM_ITEM::SYSTEM )
		{
			return IndexRangeList( topology.getAtomRange() );
		}
		else if constexpr ( ITEM == E_SYSTEM_ITEM::CHAIN )
		{
			for ( const auto & index : p_ranges )
			{
				atoms.addRange( topology.getChainAtomRange( index ) );
			}
		}
		else if constexpr ( ITEM == E_SYSTEM_ITEM::RESIDUE )
		{
			for ( const auto & index : p_ranges )
			{
				atoms.addRange( topology.getResidueAtomRange( index ) );
			}
		}
		else if constexpr ( ITEM == E_SYSTEM_ITEM::ATOM )
		{
			for ( auto it = p_ranges.rangeBegin(); it != p_ranges.rangeEnd(); it++ )
			{
				atoms.addRange( *it );
			}
		}
		else
		{
			static_assert( always_false_v<ITEM>, "Unhandled E_ITEM type in getAtomRangeList()." );
		}

		return atoms;
	}

	/**
	 * @brief Struct that describes a system item.
	 */
	struct SystemItemView
	{
		ECS::Entity					entity;
		Core::Struct::E_SYSTEM_ITEM item  = Core::Struct::E_SYSTEM_ITEM::SYSTEM;
		std::optional<Index>		index = std::nullopt;
	};

	/**
	 * @brief Get the visibility state of an item.
	 */
	App::System::E_VISIBLE_STATE getVisibleState( const SystemItemView & );

	/**
	 * @brief Get the selection state of an item.
	 */
	App::System::E_SELECTION_STATE getSelectionState( const SystemItemView & );

	/**
	 * @brief Get the color scheme of an item (nothing if multiples).
	 */
	std::optional<App::System::E_COLOR_SCHEME> getColorScheme( const SystemItemView & );

	/**
	 * @brief Count all atoms assigned in the color component.
	 */
	size_t countAssignedColorAtoms( const App::System::Color & );

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
	 */
	ECS::Entity getSystemByName( const std::string_view );

	/**
	 * @brief Retrieve a system with the corresponding pdb.
	 */
	ECS::Entity getSystemByPdb( const std::string_view );

} // namespace VTX::App::Helper::System

#endif
