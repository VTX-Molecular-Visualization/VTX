#include "app/helper/system.hpp"
#include "app/python_binding/identity.hpp"
#include "app/python_binding/topology/actions.hpp"
#include "app/python_binding/topology/binding.hpp"
#include "app/python_binding/topology/helpers.hpp"
#include <core/chemdb/atom.hpp>

namespace VTX::App::PythonBinding::Topology
{
	namespace py = pybind11;

	System Atom::getSystem() const
	{
		getTopology( entity );
		return { entity };
	}

	Residue Atom::getResidue() const { return { entity, getTopology( entity ).getAtomResidueIndex( index ) }; }

	Chain Atom::getChain() const { return { entity, getTopology( entity ).getAtomChainIndex( index ) }; }

	Category Atom::getCategory() const
	{
		const auto & topology = getTopology( entity );
		return { entity, toUnderlying( topology.getResidueCategory( topology.getAtomResidueIndex( index ) ) ) };
	}

	void bindAtom( pybind11::module_ & p_module )
	{
		bindIdentity(
			bindState<SystemItem::ATOM>( bindStyle(
				py::class_<Atom>( p_module, "Atom", "Handle to an atom of a system.", py::module_local() ),
				[]( const Atom & p_atom )
				{ return Target { p_atom.entity, SystemItem::ATOM, RangeList( p_atom.index ) }; }
			) )
		)
			.def_property_readonly(
				"index", []( const Atom & p_atom ) { return p_atom.index; }, "Index in the system."
			)
			.def_property_readonly(
				"name",
				synchronized( []( const Atom & p_atom )
							  { return getTopology( p_atom.entity ).getAtomName( p_atom.index ); } ),
				"Atom name."
			)
			.def_property_readonly(
				"symbol",
				synchronized( []( const Atom & p_atom )
							  { return getTopology( p_atom.entity ).getAtomSymbol( p_atom.index ); } ),
				"Chemical element."
			)
			.def_property_readonly(
				"symbolName",
				synchronized(
					[]( const Atom & p_atom )
					{
						const auto symbol = getTopology( p_atom.entity ).getAtomSymbol( p_atom.index );
						return std::string( Core::ChemDB::Atom::SYMBOL_NAME[ toUnderlying( symbol ) ] );
					}
				),
				"Chemical element name."
			)
			.def_property_readonly(
				"vdwRadius",
				synchronized(
					[]( const Atom & p_atom )
					{
						const auto symbol = getTopology( p_atom.entity ).getAtomSymbol( p_atom.index );
						return Core::ChemDB::Atom::SYMBOL_VDW_RADIUS[ toUnderlying( symbol ) ];
					}
				),
				"Van der Waals radius in angstroms."
			)
			.def_property_readonly(
				"originalIndex",
				synchronized( []( const Atom & p_atom )
							  { return getTopology( p_atom.entity ).getAtomOriginalIndex( p_atom.index ); } ),
				"Index in the source file."
			)
			.def_property_readonly(
				"position",
				synchronized(
					[]( const Atom & p_atom )
					{
						getTopology( p_atom.entity );
						return Helper::System::getAtomPosition( p_atom.entity, p_atom.index );
					}
				),
				"Position in the displayed frame, in system local space."
			)
			.def( "getSystem", synchronized( &Atom::getSystem ), "Return the parent system." )
			.def( "getResidue", synchronized( &Atom::getResidue ), "Return the parent residue." )
			.def( "getChain", synchronized( &Atom::getChain ), "Return the parent chain." )
			.def( "getCategory", synchronized( &Atom::getCategory ), "Return the residue category." );
	}
} // namespace VTX::App::PythonBinding::Topology
