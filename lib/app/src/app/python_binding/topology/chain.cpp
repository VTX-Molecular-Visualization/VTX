#include "app/python_binding/identity.hpp"
#include "app/python_binding/topology/actions.hpp"
#include "app/python_binding/topology/binding.hpp"
#include "app/python_binding/topology/helpers.hpp"

namespace VTX::App::PythonBinding::Topology
{
	namespace py = pybind11;

	namespace
	{
		void validateChainLocalRange( const Chain & p_chain, const Range & p_range )
		{
			if ( p_range.getLast() > getTopology( p_chain.entity ).getChainResidueCount( p_chain.index ) )
			{
				throw pybind11::index_error( "Index is outside of the chain." );
			}
		}

		void validateChainAtomLocalRange( const Chain & p_chain, const Range & p_range )
		{
			if ( p_range.getLast() > getTopology( p_chain.entity ).getChainAtomCount( p_chain.index ) )
			{
				throw pybind11::index_error( "Index is outside of the chain." );
			}
		}
	} // namespace

	System Chain::getSystem() const
	{
		getTopology( entity );
		return { entity };
	}

	ResidueCollection Chain::getResidues() const
	{ return { entity, RangeList( getTopology( entity ).getChainResidueRange( index ) ) }; }

	AtomCollection Chain::getAtoms() const
	{ return { entity, RangeList( getTopology( entity ).getChainAtomRange( index ) ) }; }

	Residue Chain::getResidue( const Index p_index ) const
	{
		validateChainLocalRange( *this, Range( p_index ) );
		return { entity, static_cast<Index>( getTopology( entity ).getChainFirstResidue( index ) + p_index ) };
	}

	Atom Chain::getAtom( const Index p_index ) const
	{
		validateChainAtomLocalRange( *this, Range( p_index ) );
		return { entity, static_cast<Index>( getTopology( entity ).getChainFirstAtom( index ) + p_index ) };
	}

	void bindChain( pybind11::module_ & p_module )
	{
		bindIdentity(
			bindState<SystemItem::CHAIN>( bindStyle(
				py::class_<Chain>( p_module, "Chain", py::module_local() ),
				[]( const Chain & p_chain )
				{ return Target { p_chain.entity, SystemItem::CHAIN, RangeList( p_chain.index ) }; }
			) )
		)
			.def_property_readonly( "index", []( const Chain & p_chain ) { return p_chain.index; } )
			.def_property_readonly(
				"name",
				synchronized( []( const Chain & p_chain )
							  { return getTopology( p_chain.entity ).getChainName( p_chain.index ); } )
			)
			.def_property_readonly(
				"residueCount",
				synchronized( []( const Chain & p_chain )
							  { return getTopology( p_chain.entity ).getChainResidueCount( p_chain.index ); } )
			)
			.def_property_readonly(
				"atomCount",
				synchronized( []( const Chain & p_chain )
							  { return getTopology( p_chain.entity ).getChainAtomCount( p_chain.index ); } )
			)
			.def_property_readonly(
				"indexFirstResidue",
				synchronized( []( const Chain & p_chain )
							  { return getTopology( p_chain.entity ).getChainFirstResidue( p_chain.index ); } )
			)
			.def_property_readonly(
				"indexLastResidue",
				synchronized(
					[]( const Chain & p_chain )
					{
						const Range range = getTopology( p_chain.entity ).getChainResidueRange( p_chain.index );
						return range.getLast();
					}
				)
			)
			.def_property_readonly(
				"indexFirstAtom",
				synchronized( []( const Chain & p_chain )
							  { return getTopology( p_chain.entity ).getChainFirstAtom( p_chain.index ); } )
			)
			.def_property_readonly(
				"indexLastAtom",
				synchronized(
					[]( const Chain & p_chain )
					{
						const Range range = getTopology( p_chain.entity ).getChainAtomRange( p_chain.index );
						return range.getLast();
					}
				)
			)
			.def( "getSystem", synchronized( &Chain::getSystem ) )
			.def( "getResidues", synchronized( &Chain::getResidues ) )
			.def( "getAtoms", synchronized( &Chain::getAtoms ) )
			.def( "getResidue", synchronized( &Chain::getResidue ), py::arg( "index" ) )
			.def( "getAtom", synchronized( &Chain::getAtom ), py::arg( "index" ) );
	}
} // namespace VTX::App::PythonBinding::Topology
