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
				py::class_<Residue>( p_module, "Residue", "Handle to a residue of a system.", py::module_local() ),
				[]( const Residue & p_residue )
				{ return Target { p_residue.entity, SystemItem::RESIDUE, RangeList( p_residue.index ) }; }
			) )
		)
			.def_property_readonly(
				"index", []( const Residue & p_residue ) { return p_residue.index; }, "Index in the system."
			)
			.def_property_readonly(
				"name",
				synchronized( []( const Residue & p_residue )
							  { return getTopology( p_residue.entity ).getResidueName( p_residue.index ); } ),
				"Residue name."
			)
			.def_property_readonly(
				"symbol",
				synchronized( []( const Residue & p_residue )
							  { return getTopology( p_residue.entity ).getResidueSymbol( p_residue.index ); } ),
				"Residue symbol."
			)
			.def_property_readonly(
				"shortName",
				synchronized(
					[]( const Residue & p_residue )
					{
						const auto symbol = getTopology( p_residue.entity ).getResidueSymbol( p_residue.index );
						return std::string( Core::ChemDB::Residue::SYMBOL_SHORT_STR[ toUnderlying( symbol ) ] );
					}
				),
				"One-letter code."
			)
			.def_property_readonly(
				"longName",
				synchronized(
					[]( const Residue & p_residue )
					{
						const auto symbol = getTopology( p_residue.entity ).getResidueSymbol( p_residue.index );
						return std::string( Core::ChemDB::Residue::SYMBOL_NAME[ toUnderlying( symbol ) ] );
					}
				),
				"Full name."
			)
			.def_property_readonly(
				"atomCount",
				synchronized( []( const Residue & p_residue )
							  { return getTopology( p_residue.entity ).getResidueAtomCount( p_residue.index ); } ),
				"Number of atoms."
			)
			.def_property_readonly(
				"indexFirstAtom",
				synchronized( []( const Residue & p_residue )
							  { return getTopology( p_residue.entity ).getResidueFirstAtom( p_residue.index ); } ),
				"System index of the first atom."
			)
			.def_property_readonly(
				"indexLastAtom",
				synchronized(
					[]( const Residue & p_residue )
					{
						const Range range = getTopology( p_residue.entity ).getResidueAtomRange( p_residue.index );
						return static_cast<Index>( range.getLast() - 1 );
					}
				),
				"System index of the last atom."
			)
			.def_property_readonly(
				"originalId",
				synchronized( []( const Residue & p_residue )
							  { return getTopology( p_residue.entity ).getResidueOriginalId( p_residue.index ); } ),
				"Residue number in the source file."
			)
			.def( "getSystem", synchronized( &Residue::getSystem ), "Return the parent system." )
			.def( "getChain", synchronized( &Residue::getChain ), "Return the parent chain." )
			.def( "getCategory", synchronized( &Residue::getCategory ), "Return the residue category." )
			.def( "getAtoms", synchronized( &Residue::getAtoms ), "Return the atoms." )
			.def(
				"getAtom",
				synchronized( &Residue::getAtom ),
				"Return an atom by its index in the residue. Raises IndexError if out of range.",
				py::arg( "index" )
			);
	}
} // namespace VTX::App::PythonBinding::Topology
