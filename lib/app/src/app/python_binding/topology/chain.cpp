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
	{
		return { entity, RangeList( getTopology( entity ).getChainResidueRange( index ) ) };
	}

	AtomCollection Chain::getAtoms() const
	{
		return { entity, RangeList( getTopology( entity ).getChainAtomRange( index ) ) };
	}

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
		py::class_<Chain>( p_module, "Chain", py::module_local() )
			.def_property_readonly( "index", []( const Chain & p_chain ) { return p_chain.index; } )
			.def_property_readonly(
				"name",
				[]( const Chain & p_chain ) { return getTopology( p_chain.entity ).getChainName( p_chain.index ); }
			)
			.def_property_readonly(
				"residueCount",
				[]( const Chain & p_chain )
				{ return getTopology( p_chain.entity ).getChainResidueCount( p_chain.index ); }
			)
			.def_property_readonly(
				"atomCount",
				[]( const Chain & p_chain ) { return getTopology( p_chain.entity ).getChainAtomCount( p_chain.index ); }
			)
			.def_property_readonly(
				"indexFirstResidue",
				[]( const Chain & p_chain )
				{ return getTopology( p_chain.entity ).getChainFirstResidue( p_chain.index ); }
			)
			.def_property_readonly(
				"indexLastResidue",
				[]( const Chain & p_chain )
				{
					const Range range = getTopology( p_chain.entity ).getChainResidueRange( p_chain.index );
					return range.getLast();
				}
			)
			.def_property_readonly(
				"indexFirstAtom",
				[]( const Chain & p_chain ) { return getTopology( p_chain.entity ).getChainFirstAtom( p_chain.index ); }
			)
			.def_property_readonly(
				"indexLastAtom",
				[]( const Chain & p_chain )
				{
					const Range range = getTopology( p_chain.entity ).getChainAtomRange( p_chain.index );
					return range.getLast();
				}
			)
			.def( "getSystem", &Chain::getSystem )
			.def( "getResidues", &Chain::getResidues )
			.def( "getAtoms", &Chain::getAtoms )
			.def( "getResidue", &Chain::getResidue, py::arg( "index" ) )
			.def( "getAtom", &Chain::getAtom, py::arg( "index" ) )
			.def(
				"setVisible",
				[]( const Chain & p_chain, const bool p_visible )
				{ setVisible<SystemItem::CHAIN>( p_chain.entity, RangeList( p_chain.index ), p_visible ); },
				py::arg( "visible" ) = true
			)
			.def(
				"setSelected",
				[]( const Chain & p_chain, const bool p_selected, const bool p_append )
				{ setSelected<SystemItem::CHAIN>( p_chain.entity, RangeList( p_chain.index ), p_selected, p_append ); },
				py::arg( "selected" ) = true,
				py::arg( "append" )	  = false
			)
			.def_property_readonly(
				"visibleState",
				[]( const Chain & p_chain )
				{ return getVisibleState( p_chain.entity, SystemItem::CHAIN, p_chain.index ); }
			)
			.def_property_readonly(
				"selectionState",
				[]( const Chain & p_chain )
				{ return getSelectionState( p_chain.entity, SystemItem::CHAIN, p_chain.index ); }
			)
			.def(
				"isVisible",
				[]( const Chain & p_chain ) { return isVisible( p_chain.entity, SystemItem::CHAIN, p_chain.index ); }
			)
			.def(
				"isFullyVisible",
				[]( const Chain & p_chain )
				{ return isFullyVisible( p_chain.entity, SystemItem::CHAIN, p_chain.index ); }
			)
			.def(
				"isSelected",
				[]( const Chain & p_chain ) { return isSelected( p_chain.entity, SystemItem::CHAIN, p_chain.index ); }
			)
			.def(
				"isFullySelected",
				[]( const Chain & p_chain )
				{ return isFullySelected( p_chain.entity, SystemItem::CHAIN, p_chain.index ); }
			);
	}
} // namespace VTX::App::PythonBinding::Topology
