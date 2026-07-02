#ifndef __VTX_APP_ACTION_COLOR_SCHEME__
#define __VTX_APP_ACTION_COLOR_SCHEME__

#include "app/ecs.hpp"
#include "app/helper/system.hpp"
#include "app/system/color.hpp"
#include <core/struct/topology.hpp>
#include <optional>

namespace VTX::App::Action::Color
{
	/**
	 * @brief Set item selection.
	 */
	template<Core::Struct::E_SYSTEM_ITEM ITEM>
	struct Add
	{
		void execute(
			const Entity							  p_ent,
			const Renderer::E_COLOR_SCHEME			  p_scheme,
			const Core::Struct::IndexRangeList &	  p_ranges	   = {},
			const std::optional<Renderer::ColorIndex> p_colorIndex = std::nullopt
		)
		{
			auto &						 reg	= REG();
			const auto &				 system = reg.get<Core::Struct::Topology>( p_ent );
			Core::Struct::IndexRangeList atoms	= Helper::System::getAtomRangeList<ITEM>( p_ent, p_ranges );
			const bool					 isCustomScheme
				= p_scheme == Renderer::E_COLOR_SCHEME::CUSTOM || p_scheme == Renderer::E_COLOR_SCHEME::CARBON_CUSTOM;

			assert(
				( isCustomScheme && p_colorIndex.has_value() ) || ( not isCustomScheme && not p_colorIndex.has_value() )
			);

			reg.patch<System::Color>(
				p_ent,
				[ &atoms, &system, p_scheme, p_colorIndex ]( System::Color & p_color )
				{
					const bool isCustomScheme = p_scheme == Renderer::E_COLOR_SCHEME::CUSTOM
												|| p_scheme == Renderer::E_COLOR_SCHEME::CARBON_CUSTOM;

					if ( isCustomScheme )
					{
						assert( p_colorIndex.has_value() );
						assert( *p_colorIndex >= Renderer::Color::LAYOUT_OFFSET_CUSTOM );
						assert(
							*p_colorIndex < Renderer::Color::LAYOUT_OFFSET_CUSTOM + Renderer::Color::LAYOUT_COUNT_CUSTOM
						);

						auto & targetColorAtoms = p_scheme == Renderer::E_COLOR_SCHEME::CUSTOM
													  ? p_color.customColorAtoms
													  : p_color.carbonCustomColorAtoms;

						if ( not targetColorAtoms.contains( *p_colorIndex ) )
						{
							targetColorAtoms.emplace( *p_colorIndex, Core::Struct::IndexRangeList() );
						}

						for ( auto & [ _, rangeList ] : p_color.colorSchemeAtoms )
						{
							rangeList.substractInPlace( atoms );
						}

						for ( auto & [ colorIndex, rangeList ] : p_color.customColorAtoms )
						{
							if ( p_scheme == Renderer::E_COLOR_SCHEME::CUSTOM && colorIndex == *p_colorIndex )
							{
								rangeList.mergeInPlace( atoms );
							}
							else
							{
								rangeList.substractInPlace( atoms );
							}
						}
						for ( auto & [ colorIndex, rangeList ] : p_color.carbonCustomColorAtoms )
						{
							if ( p_scheme == Renderer::E_COLOR_SCHEME::CARBON_CUSTOM && colorIndex == *p_colorIndex )
							{
								rangeList.mergeInPlace( atoms );
							}
							else
							{
								rangeList.substractInPlace( atoms );
							}
						}

						assert( Helper::System::countAssignedColorAtoms( p_color ) == system.getAtomCount() );
						return;
					}

					if ( not p_color.colorSchemeAtoms.contains( p_scheme ) )
					{
						p_color.colorSchemeAtoms.emplace( p_scheme, Core::Struct::IndexRangeList() );
					}

					size_t count = 0;
					for ( auto & [ scheme, rangeList ] : p_color.colorSchemeAtoms )
					{
						if ( scheme == p_scheme )
						{
							rangeList.mergeInPlace( atoms );
						}
						else
						{
							rangeList.substractInPlace( atoms );
						}

						count += rangeList.count();
					}
					for ( auto & [ _, rangeList ] : p_color.customColorAtoms )
					{
						rangeList.substractInPlace( atoms );
						count += rangeList.count();
					}
					for ( auto & [ _, rangeList ] : p_color.carbonCustomColorAtoms )
					{
						rangeList.substractInPlace( atoms );
						count += rangeList.count();
					}

					assert( count == system.getAtomCount() );
				}
			);
		}

		void execute(
			const Entity							  p_ent,
			const Renderer::E_COLOR_SCHEME			  p_scheme,
			const Core::Struct::IndexRange &		  p_range,
			const std::optional<Renderer::ColorIndex> p_colorIndex = std::nullopt
		)
		{ execute( p_ent, p_scheme, Core::Struct::IndexRangeList( p_range ), p_colorIndex ); }

		void execute(
			const Entity							  p_ent,
			const Renderer::E_COLOR_SCHEME			  p_scheme,
			const std::vector<Index> &				  p_values,
			const std::optional<Renderer::ColorIndex> p_colorIndex = std::nullopt
		)
		{ execute( p_ent, p_scheme, Core::Struct::IndexRangeList( p_values ), p_colorIndex ); }

		void execute(
			const Entity							  p_ent,
			const Renderer::E_COLOR_SCHEME			  p_scheme,
			const Index								  p_value,
			const std::optional<Renderer::ColorIndex> p_colorIndex = std::nullopt
		)
		{ execute( p_ent, p_scheme, Core::Struct::IndexRangeList( p_value ), p_colorIndex ); }
	};

	struct AddItem
	{
		void execute(
			const Entity							  p_ent,
			const Core::Struct::E_SYSTEM_ITEM		  p_item,
			const Renderer::E_COLOR_SCHEME			  p_scheme,
			const Core::Struct::IndexRangeList &	  p_ranges	   = {},
			const std::optional<Renderer::ColorIndex> p_colorIndex = std::nullopt
		)
		{
			switch ( p_item )
			{
			case Core::Struct::E_SYSTEM_ITEM::SYSTEM:
				Add<Core::Struct::E_SYSTEM_ITEM::SYSTEM>().execute( p_ent, p_scheme, p_ranges, p_colorIndex );
				break;
			case Core::Struct::E_SYSTEM_ITEM::CATEGORY:
				Add<Core::Struct::E_SYSTEM_ITEM::CATEGORY>().execute( p_ent, p_scheme, p_ranges, p_colorIndex );
				break;
			case Core::Struct::E_SYSTEM_ITEM::CHAIN:
				Add<Core::Struct::E_SYSTEM_ITEM::CHAIN>().execute( p_ent, p_scheme, p_ranges, p_colorIndex );
				break;
			case Core::Struct::E_SYSTEM_ITEM::RESIDUE:
				Add<Core::Struct::E_SYSTEM_ITEM::RESIDUE>().execute( p_ent, p_scheme, p_ranges, p_colorIndex );
				break;
			case Core::Struct::E_SYSTEM_ITEM::ATOM:
				Add<Core::Struct::E_SYSTEM_ITEM::ATOM>().execute( p_ent, p_scheme, p_ranges, p_colorIndex );
				break;
			default: assert( false && "Unhandled E_SYSTEM_ITEM type in AddItem action." ); break;
			}
		}

		void execute(
			const Entity							  p_ent,
			const Core::Struct::E_SYSTEM_ITEM		  p_item,
			const Renderer::E_COLOR_SCHEME			  p_scheme,
			const Core::Struct::IndexRange &		  p_range,
			const std::optional<Renderer::ColorIndex> p_colorIndex = std::nullopt
		)
		{ execute( p_ent, p_item, p_scheme, Core::Struct::IndexRangeList( p_range ), p_colorIndex ); }

		void execute(
			const Entity							  p_ent,
			const Core::Struct::E_SYSTEM_ITEM		  p_item,
			const Renderer::E_COLOR_SCHEME			  p_scheme,
			const std::vector<Index> &				  p_values,
			const std::optional<Renderer::ColorIndex> p_colorIndex = std::nullopt
		)
		{ execute( p_ent, p_item, p_scheme, Core::Struct::IndexRangeList( p_values ), p_colorIndex ); }

		void execute(
			const Entity							  p_ent,
			const Core::Struct::E_SYSTEM_ITEM		  p_item,
			const Renderer::E_COLOR_SCHEME			  p_scheme,
			const Index								  p_value,
			const std::optional<Renderer::ColorIndex> p_colorIndex = std::nullopt
		)
		{ execute( p_ent, p_item, p_scheme, Core::Struct::IndexRangeList( p_value ), p_colorIndex ); }
	};

	struct AddSelected
	{
		void execute(
			const Renderer::E_COLOR_SCHEME			  p_scheme,
			const std::optional<Renderer::ColorIndex> p_colorIndex = std::nullopt
		)
		{
			REG().view<System::Selection, System::Color>().each(
				[ p_scheme, p_colorIndex ](
					const Entity p_ent, const System::Selection & p_selection, const System::Color & p_color
				)
				{
					const Core::Struct::IndexRangeList ranges = p_selection.atoms.toRangeList<Index>();
					if ( ranges.isEmpty() )
					{
						return;
					}

					Add<Core::Struct::E_SYSTEM_ITEM::ATOM>().execute( p_ent, p_scheme, ranges, p_colorIndex );
				}
			);
		}
	};

	template<Core::Struct::E_SYSTEM_ITEM ITEM>
	struct AddSecondaryStructure
	{
		void execute(
			const Entity									   p_ent,
			const Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE p_scheme,
			const Core::Struct::IndexRangeList &			   p_ranges		= {},
			const std::optional<Renderer::ColorIndex>		   p_colorIndex = std::nullopt
		)
		{
			auto &						 reg	  = REG();
			const auto &				 system	  = reg.get<Core::Struct::Topology>( p_ent );
			Core::Struct::IndexRangeList residues = Helper::System::getResidueRangeList<ITEM>( p_ent, p_ranges );
			const bool isCustomScheme			  = p_scheme == Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE::CUSTOM;

			assert(
				( isCustomScheme && p_colorIndex.has_value() ) || ( not isCustomScheme && not p_colorIndex.has_value() )
			);

			reg.patch<System::Color>(
				p_ent,
				[ &residues, &system, p_scheme, p_colorIndex ]( System::Color & p_color )
				{
					const bool isCustomScheme = p_scheme == Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE::CUSTOM;

					if ( isCustomScheme )
					{
						assert( p_colorIndex.has_value() );
						assert( *p_colorIndex >= Renderer::Color::LAYOUT_OFFSET_CUSTOM );
						assert(
							*p_colorIndex < Renderer::Color::LAYOUT_OFFSET_CUSTOM + Renderer::Color::LAYOUT_COUNT_CUSTOM
						);

						if ( not p_color.customSecondaryStructureColorResidues.contains( *p_colorIndex ) )
						{
							p_color.customSecondaryStructureColorResidues.emplace(
								*p_colorIndex, Core::Struct::IndexRangeList()
							);
						}

						for ( auto & [ _, rangeList ] : p_color.colorSchemeSecondaryStructureResidues )
						{
							rangeList.substractInPlace( residues );
						}

						for ( auto & [ colorIndex, rangeList ] : p_color.customSecondaryStructureColorResidues )
						{
							if ( colorIndex == *p_colorIndex )
							{
								rangeList.mergeInPlace( residues );
							}
							else
							{
								rangeList.substractInPlace( residues );
							}
						}

						assert(
							Helper::System::countAssignedSecondaryStructureResidues( p_color )
							== system.getResidueCount()
						);
						return;
					}

					if ( not p_color.colorSchemeSecondaryStructureResidues.contains( p_scheme ) )
					{
						p_color.colorSchemeSecondaryStructureResidues.emplace(
							p_scheme, Core::Struct::IndexRangeList()
						);
					}

					size_t count = 0;
					for ( auto & [ scheme, rangeList ] : p_color.colorSchemeSecondaryStructureResidues )
					{
						if ( scheme == p_scheme )
						{
							rangeList.mergeInPlace( residues );
						}
						else
						{
							rangeList.substractInPlace( residues );
						}

						count += rangeList.count();
					}
					for ( auto & [ _, rangeList ] : p_color.customSecondaryStructureColorResidues )
					{
						rangeList.substractInPlace( residues );
						count += rangeList.count();
					}

					assert( count == system.getResidueCount() );
				}
			);
		}

		void execute(
			const Entity									   p_ent,
			const Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE p_scheme,
			const Core::Struct::IndexRange &				   p_range,
			const std::optional<Renderer::ColorIndex>		   p_colorIndex = std::nullopt
		)
		{ execute( p_ent, p_scheme, Core::Struct::IndexRangeList( p_range ), p_colorIndex ); }

		void execute(
			const Entity									   p_ent,
			const Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE p_scheme,
			const std::vector<Index> &						   p_values,
			const std::optional<Renderer::ColorIndex>		   p_colorIndex = std::nullopt
		)
		{ execute( p_ent, p_scheme, Core::Struct::IndexRangeList( p_values ), p_colorIndex ); }

		void execute(
			const Entity									   p_ent,
			const Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE p_scheme,
			const Index										   p_value,
			const std::optional<Renderer::ColorIndex>		   p_colorIndex = std::nullopt
		)
		{ execute( p_ent, p_scheme, Core::Struct::IndexRangeList( p_value ), p_colorIndex ); }
	};

	struct AddSecondaryStructureItem
	{
		void execute(
			const Entity									   p_ent,
			const Core::Struct::E_SYSTEM_ITEM				   p_item,
			const Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE p_scheme,
			const Core::Struct::IndexRangeList &			   p_ranges		= {},
			const std::optional<Renderer::ColorIndex>		   p_colorIndex = std::nullopt
		)
		{
			switch ( p_item )
			{
			case Core::Struct::E_SYSTEM_ITEM::SYSTEM:
				AddSecondaryStructure<Core::Struct::E_SYSTEM_ITEM::SYSTEM>().execute(
					p_ent, p_scheme, p_ranges, p_colorIndex
				);
				break;
			case Core::Struct::E_SYSTEM_ITEM::CATEGORY:
				AddSecondaryStructure<Core::Struct::E_SYSTEM_ITEM::CATEGORY>().execute(
					p_ent, p_scheme, p_ranges, p_colorIndex
				);
				break;
			case Core::Struct::E_SYSTEM_ITEM::CHAIN:
				AddSecondaryStructure<Core::Struct::E_SYSTEM_ITEM::CHAIN>().execute(
					p_ent, p_scheme, p_ranges, p_colorIndex
				);
				break;
			case Core::Struct::E_SYSTEM_ITEM::RESIDUE:
				AddSecondaryStructure<Core::Struct::E_SYSTEM_ITEM::RESIDUE>().execute(
					p_ent, p_scheme, p_ranges, p_colorIndex
				);
				break;
			case Core::Struct::E_SYSTEM_ITEM::ATOM:
				AddSecondaryStructure<Core::Struct::E_SYSTEM_ITEM::ATOM>().execute(
					p_ent, p_scheme, p_ranges, p_colorIndex
				);
				break;
			default: assert( false && "Unhandled E_SYSTEM_ITEM type in AddSecondaryStructureItem action." ); break;
			}
		}

		void execute(
			const Entity									   p_ent,
			const Core::Struct::E_SYSTEM_ITEM				   p_item,
			const Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE p_scheme,
			const Core::Struct::IndexRange &				   p_range,
			const std::optional<Renderer::ColorIndex>		   p_colorIndex = std::nullopt
		)
		{ execute( p_ent, p_item, p_scheme, Core::Struct::IndexRangeList( p_range ), p_colorIndex ); }

		void execute(
			const Entity									   p_ent,
			const Core::Struct::E_SYSTEM_ITEM				   p_item,
			const Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE p_scheme,
			const std::vector<Index> &						   p_values,
			const std::optional<Renderer::ColorIndex>		   p_colorIndex = std::nullopt
		)
		{ execute( p_ent, p_item, p_scheme, Core::Struct::IndexRangeList( p_values ), p_colorIndex ); }

		void execute(
			const Entity									   p_ent,
			const Core::Struct::E_SYSTEM_ITEM				   p_item,
			const Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE p_scheme,
			const Index										   p_value,
			const std::optional<Renderer::ColorIndex>		   p_colorIndex = std::nullopt
		)
		{ execute( p_ent, p_item, p_scheme, Core::Struct::IndexRangeList( p_value ), p_colorIndex ); }
	};

} // namespace VTX::App::Action::Color

#endif
