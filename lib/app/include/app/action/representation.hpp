#ifndef __VTX_APP_ACTION_REPRESENTATION__
#define __VTX_APP_ACTION_REPRESENTATION__

#include "app/ecs.hpp"
#include "app/helper/system.hpp"
#include "app/system/representation.hpp"
#include <core/struct/system.hpp>
#include <renderer/representation.hpp>
#include <util/type_traits.hpp>

namespace VTX::App::Action::Representation
{

	/**
	 * @brief Add a representation to a system.
	 */
	template<Core::Struct::E_SYSTEM_ITEM ITEM>
	struct Add
	{
		void execute(
			const ECS::Entity					 p_ent,
			const ECS::Entity					 p_preset,
			const Core::Struct::IndexRangeList & p_ranges = {}
		)
		{
			auto &						 reg	= REG();
			const auto &				 system = reg.get<Core::Struct::Topology>( p_ent );
			Core::Struct::IndexRangeList atoms	= Helper::System::getAtomRangeList<ITEM>( p_ent, p_ranges );

			reg.patch<System::Representation>(
				p_ent,
				[ &atoms, &system, p_preset ]( System::Representation & p_representation )
				{
					// Merges ranges.
					if ( not p_representation.presetAtoms.contains( p_preset ) )
					{
						p_representation.presetAtoms.emplace( p_preset, Core::Struct::IndexRangeList() );
					}

					size_t count = 0;
					for ( auto & [ entity, rangeList ] : p_representation.presetAtoms )
					{
						if ( entity == p_preset )
						{
							rangeList.mergeInPlace( atoms );
						}
						else
						{
							rangeList.substractInPlace( atoms );
						}

						count += rangeList.count();
					}

					assert( count == system.getAtomCount() );
				}
			);
		}

		void execute( const ECS::Entity p_ent, const ECS::Entity p_preset, const Core::Struct::IndexRange & p_range )
		{
			execute( p_ent, p_preset, Core::Struct::IndexRangeList( p_range ) );
		}

		void execute( const ECS::Entity p_ent, const ECS::Entity p_preset, const std::vector<Index> & p_values )
		{
			execute( p_ent, p_preset, Core::Struct::IndexRangeList( p_values ) );
		}

		void execute( const ECS::Entity p_ent, const ECS::Entity p_preset, const Index p_value )
		{
			execute( p_ent, p_preset, Core::Struct::IndexRangeList( p_value ) );
		}
	};

	struct AddItem
	{
		void execute(
			const ECS::Entity					 p_ent,
			const Core::Struct::E_SYSTEM_ITEM	 p_item,
			const ECS::Entity					 p_preset,
			const Core::Struct::IndexRangeList & p_ranges = {}
		)
		{
			switch ( p_item )
			{
			case Core::Struct::E_SYSTEM_ITEM::SYSTEM:
				Add<Core::Struct::E_SYSTEM_ITEM::SYSTEM>().execute( p_ent, p_preset, p_ranges );
				break;
			case Core::Struct::E_SYSTEM_ITEM::CHAIN:
				Add<Core::Struct::E_SYSTEM_ITEM::CHAIN>().execute( p_ent, p_preset, p_ranges );
				break;
			case Core::Struct::E_SYSTEM_ITEM::RESIDUE:
				Add<Core::Struct::E_SYSTEM_ITEM::RESIDUE>().execute( p_ent, p_preset, p_ranges );
				break;
			case Core::Struct::E_SYSTEM_ITEM::ATOM:
				Add<Core::Struct::E_SYSTEM_ITEM::ATOM>().execute( p_ent, p_preset, p_ranges );
				break;
			default: assert( false && "Unhandled E_SYSTEM_ITEM type in AddItem action." ); break;
			}
		}

		void execute(
			const ECS::Entity				  p_ent,
			const Core::Struct::E_SYSTEM_ITEM p_item,
			const ECS::Entity				  p_preset,
			const Core::Struct::IndexRange &  p_range
		)
		{
			execute( p_ent, p_item, p_preset, Core::Struct::IndexRangeList( p_range ) );
		}

		void execute(
			const ECS::Entity				  p_ent,
			const Core::Struct::E_SYSTEM_ITEM p_item,
			const ECS::Entity				  p_preset,
			const std::vector<Index> &		  p_values
		)
		{
			execute( p_ent, p_item, p_preset, Core::Struct::IndexRangeList( p_values ) );
		}

		void execute(
			const ECS::Entity				  p_ent,
			const Core::Struct::E_SYSTEM_ITEM p_item,
			const ECS::Entity				  p_preset,
			const Index						  p_value
		)
		{
			execute( p_ent, p_item, p_preset, Core::Struct::IndexRangeList( p_value ) );
		}
	};

	/**
	 * @brief Change a representation value.
	 */
	template<VTX::Renderer::E_REPRESENTATION_VALUES S, typename T>
	struct Change
	{
		void execute( const ECS::Entity p_e, const T & p_value )
		{
			REG().patch<Renderer::Representation>(
				p_e,
				[ p_value ]( Renderer::Representation & p_representation )
				{
					if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::HAS_SPHERE )
					{
						p_representation.hasSphere = p_value;
					}
					else if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::RADIUS_SPHERE_FIXED )
					{
						p_representation.radiusSphereFixed = p_value;
					}
					else if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::RADIUS_SPHERE_ADD )
					{
						p_representation.radiusSphereAdd = p_value;
					}
					else if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::IS_SPHERE_RADIUS_FIXED )
					{
						p_representation.isRadiusSphereFixed = p_value;
					}
					else if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::HAS_CYLINDER )
					{
						p_representation.hasCylinder = p_value;
					}
					else if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::RADIUS_CYLINDER )
					{
						p_representation.radiusCylinder = p_value;
					}
					else if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::CYLINDER_COLOR_BLENDING )
					{
						p_representation.cylinderColorBlending = p_value;
					}
					else if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::HAS_RIBBON )
					{
						p_representation.hasRibbon = p_value;
					}
					else if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::RIBBON_COLOR_BLENDING )
					{
						p_representation.ribbonColorBlending = p_value;
					}
					else if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::HAS_SES )
					{
						p_representation.hasSes = p_value;
					}
					else if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::SES_PROBE_RADIUS )
					{
						p_representation.sesProbeRadius = p_value;
					}
					else
					{
						static_assert( always_false_v<S>, "Representation::Change: invalid representation value." );
					}
				}
			);
		}
	};
} // namespace VTX::App::Action::Representation
#endif
