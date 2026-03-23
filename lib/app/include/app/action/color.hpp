#ifndef __VTX_APP_ACTION_COLOR_SCHEME__
#define __VTX_APP_ACTION_COLOR_SCHEME__

#include "app/ecs.hpp"
#include "app/helper/system.hpp"
#include "app/system/color.hpp"
#include "app/system/uid.hpp"
#include <core/struct/topology.hpp>

namespace VTX::App::Action::Color
{

	/**
	 * @brief Set item selection.
	 */
	template<Core::Struct::E_SYSTEM_ITEM ITEM>
	struct Add
	{
		void execute(
			const ECS::Entity					 p_ent,
			const System::E_COLOR_SCHEME		 p_scheme,
			const Core::Struct::IndexRangeList & p_ranges = {}
		)
		{
			auto &						 reg	= REG();
			const auto &				 system = reg.get<Core::Struct::Topology>( p_ent );
			Core::Struct::IndexRangeList atoms	= Helper::System::getAtomRangeList<ITEM>( p_ent, p_ranges );

			reg.patch<System::Color>(
				p_ent,
				[ &atoms, &system, p_scheme ]( System::Color & p_color )
				{
					// Merge ranges.
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

					assert( count == system.getAtomCount() );
				}
			);
		}

		void execute(
			const ECS::Entity				 p_ent,
			const System::E_COLOR_SCHEME	 p_scheme,
			const Core::Struct::IndexRange & p_range
		)
		{
			execute( p_ent, p_scheme, Core::Struct::IndexRangeList( p_range ) );
		}

		void execute( const ECS::Entity p_ent, System::E_COLOR_SCHEME p_scheme, const std::vector<Index> & p_values )
		{
			execute( p_ent, p_scheme, Core::Struct::IndexRangeList( p_values ) );
		}

		void execute( const ECS::Entity p_ent, System::E_COLOR_SCHEME p_scheme, const Index p_value )
		{
			execute( p_ent, p_scheme, Core::Struct::IndexRangeList( p_value ) );
		}
	};

	struct AddItem
	{
		void execute(
			const ECS::Entity					 p_ent,
			const Core::Struct::E_SYSTEM_ITEM	 p_item,
			const System::E_COLOR_SCHEME		 p_scheme,
			const Core::Struct::IndexRangeList & p_ranges = {}
		)
		{
			switch ( p_item )
			{
			case Core::Struct::E_SYSTEM_ITEM::SYSTEM:
				Add<Core::Struct::E_SYSTEM_ITEM::SYSTEM>().execute( p_ent, p_scheme, p_ranges );
				break;
			case Core::Struct::E_SYSTEM_ITEM::CHAIN:
				Add<Core::Struct::E_SYSTEM_ITEM::CHAIN>().execute( p_ent, p_scheme, p_ranges );
				break;
			case Core::Struct::E_SYSTEM_ITEM::RESIDUE:
				Add<Core::Struct::E_SYSTEM_ITEM::RESIDUE>().execute( p_ent, p_scheme, p_ranges );
				break;
			case Core::Struct::E_SYSTEM_ITEM::ATOM:
				Add<Core::Struct::E_SYSTEM_ITEM::ATOM>().execute( p_ent, p_scheme, p_ranges );
				break;
			default: assert( false && "Unhandled E_SYSTEM_ITEM type in AddItem action." ); break;
			}
		}

		void execute(
			const ECS::Entity				  p_ent,
			const Core::Struct::E_SYSTEM_ITEM p_item,
			const System::E_COLOR_SCHEME	  p_scheme,
			const Core::Struct::IndexRange &  p_range
		)
		{
			execute( p_ent, p_item, p_scheme, Core::Struct::IndexRangeList( p_range ) );
		}

		void execute(
			const ECS::Entity				  p_ent,
			const Core::Struct::E_SYSTEM_ITEM p_item,
			const System::E_COLOR_SCHEME	  p_scheme,
			const std::vector<Index> &		  p_values
		)
		{
			execute( p_ent, p_item, p_scheme, Core::Struct::IndexRangeList( p_values ) );
		}

		void execute(
			const ECS::Entity				  p_ent,
			const Core::Struct::E_SYSTEM_ITEM p_item,
			const System::E_COLOR_SCHEME	  p_scheme,
			const Index						  p_value
		)
		{
			execute( p_ent, p_item, p_scheme, Core::Struct::IndexRangeList( p_value ) );
		}
	};

} // namespace VTX::App::Action::Color

#endif
