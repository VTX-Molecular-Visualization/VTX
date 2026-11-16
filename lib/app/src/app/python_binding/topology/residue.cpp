#include "app/python_binding/identity.hpp"
#include "app/python_binding/topology/actions.hpp"
#include "app/python_binding/topology/binding.hpp"
#include "app/python_binding/topology/helpers.hpp"
#include <core/chemdb/residue.hpp>

namespace VTX::App::PythonBinding::Topology
{
	namespace py = pybind11;

	namespace
	{
		void validateResidueAtomLocalRange( const Residue & p_residue, const Range & p_range )
		{
			if ( p_range.getLast() > getTopology( p_residue.entity ).getResidueAtomCount( p_residue.index ) )
			{
				throw pybind11::index_error( "Index is outside of the residue." );
			}
		}
	} // namespace

	System Residue::getSystem() const
	{
		getTopology( entity );
		return { entity };
	}

	Chain Residue::getChain() const { return { entity, getTopology( entity ).getResidueChainIndex( index ) }; }

	Category Residue::getCategory() const
	{ return { entity, toUnderlying( getTopology( entity ).getResidueCategory( index ) ) }; }

	AtomCollection Residue::getAtoms() const
	{ return { entity, RangeList( getTopology( entity ).getResidueAtomRange( index ) ) }; }

	Atom Residue::getAtom( const Index p_index ) const
	{
		validateResidueAtomLocalRange( *this, Range( p_index ) );
		return { entity, static_cast<Index>( getTopology( entity ).getResidueFirstAtom( index ) + p_index ) };
	}

	void bindResidue( pybind11::module_ & p_module )
	{
		bindIdentity(
			bindState<SystemItem::RESIDUE>( bindStyle(
				py::class_<Residue>( p_module, "Residue", py::module_local() ),
				[]( const Residue & p_residue )
				{ return Target { p_residue.entity, SystemItem::RESIDUE, RangeList( p_residue.index ) }; }
			) )
		)
			.def_property_readonly( "index", []( const Residue & p_residue ) { return p_residue.index; } )
			.def_property_readonly(
				"name",
				synchronized( []( const Residue & p_residue )
							  { return getTopology( p_residue.entity ).getResidueName( p_residue.index ); } )
			)
			.def_property_readonly(
				"symbol",
				synchronized( []( const Residue & p_residue )
							  { return getTopology( p_residue.entity ).getResidueSymbol( p_residue.index ); } )
			)
			.def_property_readonly(
				"shortName",
				synchronized(
					[]( const Residue & p_residue )
					{
						const auto symbol = getTopology( p_residue.entity ).getResidueSymbol( p_residue.index );
						return std::string( Core::ChemDB::Residue::SYMBOL_SHORT_STR[ toUnderlying( symbol ) ] );
					}
				)
			)
			.def_property_readonly(
				"longName",
				synchronized(
					[]( const Residue & p_residue )
					{
						const auto symbol = getTopology( p_residue.entity ).getResidueSymbol( p_residue.index );
						return std::string( Core::ChemDB::Residue::SYMBOL_NAME[ toUnderlying( symbol ) ] );
					}
				)
			)
			.def_property_readonly(
				"atomCount",
				synchronized( []( const Residue & p_residue )
							  { return getTopology( p_residue.entity ).getResidueAtomCount( p_residue.index ); } )
			)
			.def_property_readonly(
				"indexFirstAtom",
				synchronized( []( const Residue & p_residue )
							  { return getTopology( p_residue.entity ).getResidueFirstAtom( p_residue.index ); } )
			)
			.def_property_readonly(
				"indexLastAtom",
				synchronized(
					[]( const Residue & p_residue )
					{
						const Range range = getTopology( p_residue.entity ).getResidueAtomRange( p_residue.index );
						return range.getLast();
					}
				)
			)
			.def_property_readonly(
				"originalId",
				synchronized( []( const Residue & p_residue )
							  { return getTopology( p_residue.entity ).getResidueOriginalId( p_residue.index ); } )
			)
			.def( "getSystem", synchronized( &Residue::getSystem ) )
			.def( "getChain", synchronized( &Residue::getChain ) )
			.def( "getCategory", synchronized( &Residue::getCategory ) )
			.def( "getAtoms", synchronized( &Residue::getAtoms ) )
			.def( "getAtom", synchronized( &Residue::getAtom ), py::arg( "index" ) );
	}
} // namespace VTX::App::PythonBinding::Topology
