#include "app/python_binding/topology/collection.hpp"
#include "app/helper/trajectory.hpp"
#include "app/python_binding/topology/binding.hpp"
#include "app/python_binding/topology/helpers.hpp"
#include <pybind11/stl.h>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace VTX::App::PythonBinding::Topology
{
	namespace
	{
		std::vector<Vec3f> _getAtomPositions( const AtomCollection & p_collection )
		{
			getTopology( p_collection.entity );
			std::vector<Vec3f> positions;
			positions.reserve( p_collection.ranges.count() );
			const bool available = App::Helper::Trajectory::visitCurrentFrame(
				p_collection.entity,
				[ & ]( const Core::Struct::FrameView p_positions )
				{
					for ( const Index atom : p_collection.ranges )
					{
						if ( atom >= p_positions.size() )
						{
							throw std::out_of_range( "Atom position is unavailable." );
						}
						positions.push_back( p_positions[ atom ] );
					}
				}
			);
			if ( not available )
			{
				throw std::out_of_range( "Atom position is unavailable." );
			}
			return positions;
		}

		template<typename T, typename Getter>
		auto _collect( const Collection<T> & p_collection, const Getter & p_getter )
		{
			const Core::Struct::Topology & topology = getTopology( p_collection.entity );
			std::vector<
				std::remove_cvref_t<std::invoke_result_t<const Getter &, const Core::Struct::Topology &, Index>>>
				values;
			values.reserve( p_collection.ranges.count() );
			for ( const Index index : p_collection.ranges )
			{
				values.emplace_back( p_getter( topology, index ) );
			}
			return values;
		}
	} // namespace

	void bindCollections( pybind11::module_ & p_module )
	{
		bindCollection<Chain>( p_module, "ChainCollection", SystemItem::CHAIN )
			.def_property_readonly(
				"names",
				synchronized(
					[]( const ChainCollection & p_collection )
					{
						return _collect(
							p_collection,
							[]( const Core::Struct::Topology & p_topology, const Index p_index )
							{ return p_topology.getChainName( p_index ); }
						);
					}
				),
				"Names of all chains, fetched in a single call."
			);
		bindCollection<Residue>( p_module, "ResidueCollection", SystemItem::RESIDUE )
			.def_property_readonly(
				"names",
				synchronized(
					[]( const ResidueCollection & p_collection )
					{
						return _collect(
							p_collection,
							[]( const Core::Struct::Topology & p_topology, const Index p_index )
							{ return p_topology.getResidueName( p_index ); }
						);
					}
				),
				"Names of all residues, fetched in a single call."
			)
			.def_property_readonly(
				"symbols",
				synchronized(
					[]( const ResidueCollection & p_collection )
					{
						return _collect(
							p_collection,
							[]( const Core::Struct::Topology & p_topology, const Index p_index )
							{ return p_topology.getResidueSymbol( p_index ); }
						);
					}
				),
				"Symbols of all residues, fetched in a single call."
			);
		bindCollection<Atom>( p_module, "AtomCollection", SystemItem::ATOM )
			.def_property_readonly(
				"positions",
				synchronized( &_getAtomPositions ),
				"Current frame positions of all atoms, fetched in a single call."
			)
			.def_property_readonly(
				"names",
				synchronized(
					[]( const AtomCollection & p_collection )
					{
						return _collect(
							p_collection,
							[]( const Core::Struct::Topology & p_topology, const Index p_index )
							{ return p_topology.getAtomName( p_index ); }
						);
					}
				),
				"Names of all atoms, fetched in a single call."
			)
			.def_property_readonly(
				"symbols",
				synchronized(
					[]( const AtomCollection & p_collection )
					{
						return _collect(
							p_collection,
							[]( const Core::Struct::Topology & p_topology, const Index p_index )
							{ return p_topology.getAtomSymbol( p_index ); }
						);
					}
				),
				"Symbols of all atoms, fetched in a single call."
			);
		bindCollection<Bond>( p_module, "BondCollection" );
		bindCollection<Category>( p_module, "CategoryCollection", SystemItem::CATEGORY );
	}
} // namespace VTX::App::PythonBinding::Topology
