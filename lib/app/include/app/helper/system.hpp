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
	Core::Struct::IndexRange getAtomRange( const Entity p_ent, const std::optional<Index> p_index = std::nullopt )
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
		const Entity						 p_ent,
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
		else if constexpr ( ITEM == E_SYSTEM_ITEM::CATEGORY )
		{
			for ( const auto & index : p_ranges )
			{
				atoms.mergeInPlace( topology.getCategoryAtomRangeList( index ) );
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
			return p_ranges;
		}
		else
		{
			static_assert( always_false_v<ITEM>, "Unhandled E_ITEM type in getAtomRangeList()." );
		}

		return atoms;
	}

	/**
	 * @brief Get the residue range list of an item.
	 */
	template<Core::Struct::E_SYSTEM_ITEM ITEM>
	Core::Struct::IndexRangeList getResidueRangeList(
		const Entity						 p_ent,
		const Core::Struct::IndexRangeList & p_ranges = {}
	)
	{
		using namespace Core::Struct;

		const auto &   topology = REG().get<Core::Struct::Topology>( p_ent );
		IndexRangeList residues;

		if constexpr ( ITEM == E_SYSTEM_ITEM::SYSTEM )
		{
			return IndexRangeList( IndexRange::fromFirstCount( 0, topology.getResidueCount() ) );
		}
		else if constexpr ( ITEM == E_SYSTEM_ITEM::CHAIN )
		{
			for ( const auto & index : p_ranges )
			{
				residues.addRange( topology.getChainResidueRange( index ) );
			}
		}
		else if constexpr ( ITEM == E_SYSTEM_ITEM::CATEGORY )
		{
			for ( const auto & index : p_ranges )
			{
				for ( const Index residue :
					  topology.getCategoryResidues( static_cast<Core::ChemDB::Category::TYPE>( index ) ) )
				{
					residues.addRange( IndexRange( residue ) );
				}
			}
		}
		else if constexpr ( ITEM == E_SYSTEM_ITEM::RESIDUE )
		{
			return p_ranges;
		}
		else if constexpr ( ITEM == E_SYSTEM_ITEM::ATOM )
		{
			for ( const auto & index : p_ranges )
			{
				residues.addRange( IndexRange( topology.getAtomResidueIndex( index ) ) );
			}
		}
		else
		{
			static_assert( always_false_v<ITEM>, "Unhandled E_ITEM type in getResidueRangeList()." );
		}

		return residues;
	}

	/**
	 * @brief Struct that describes a system item.
	 */
	struct SystemItemView
	{
		Entity						entity;
		Core::Struct::E_SYSTEM_ITEM item  = Core::Struct::E_SYSTEM_ITEM::SYSTEM;
		std::optional<Index>		index = std::nullopt;
	};

	/**
	 * @brief Get current atom position in system-local coordinates.
	 */
	Vec3f getAtomPosition( const Entity p_entity, const Index p_atom );

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
	std::optional<Renderer::E_COLOR_SCHEME> getColorScheme( const SystemItemView & );

	/**
	 * @brief Get the secondary structure color scheme of an item (nothing if multiples).
	 */
	std::optional<Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE> getSecondaryStructureColorScheme(
		const SystemItemView &
	);

	/**
	 * @brief Count all atoms assigned in the color component.
	 */
	size_t countAssignedColorAtoms( const App::System::Color & );

	/**
	 * @brief Count all residues assigned in the secondary structure color component.
	 */
	size_t countAssignedSecondaryStructureResidues( const App::System::Color & );

	/**
	 * @brief Check if an item is a color scheme root.
	 */
	bool isColorSchemeRoot( const SystemItemView & );

	/**
	 * @brief Get the representation of an item (nothing if multiples).
	 */
	std::optional<Entity> getRepresentation( const SystemItemView & );

	/**
	 * @brief Check if an item is a representation root.
	 */
	bool isRepresentationRoot( const SystemItemView & );

} // namespace VTX::App::Helper::System

#endif
