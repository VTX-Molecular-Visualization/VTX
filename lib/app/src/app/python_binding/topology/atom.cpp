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
				py::class_<Atom>( p_module, "Atom", py::module_local() ),
				[]( const Atom & p_atom )
				{ return Target { p_atom.entity, SystemItem::ATOM, RangeList( p_atom.index ) }; }
			) )
		)
			.def_property_readonly( "index", []( const Atom & p_atom ) { return p_atom.index; } )
			.def_property_readonly(
				"name",
				synchronized( []( const Atom & p_atom )
							  { return getTopology( p_atom.entity ).getAtomName( p_atom.index ); } )
			)
			.def_property_readonly(
				"symbol",
				synchronized( []( const Atom & p_atom )
							  { return getTopology( p_atom.entity ).getAtomSymbol( p_atom.index ); } )
			)
			.def_property_readonly(
				"symbolName",
				synchronized(
					[]( const Atom & p_atom )
					{
						const auto symbol = getTopology( p_atom.entity ).getAtomSymbol( p_atom.index );
						return std::string( Core::ChemDB::Atom::SYMBOL_NAME[ toUnderlying( symbol ) ] );
					}
				)
			)
			.def_property_readonly(
				"vdwRadius",
				synchronized(
					[]( const Atom & p_atom )
					{
						const auto symbol = getTopology( p_atom.entity ).getAtomSymbol( p_atom.index );
						return Core::ChemDB::Atom::SYMBOL_VDW_RADIUS[ toUnderlying( symbol ) ];
					}
				)
			)
			.def_property_readonly(
				"originalIndex",
				synchronized( []( const Atom & p_atom )
							  { return getTopology( p_atom.entity ).getAtomOriginalIndex( p_atom.index ); } )
			)
			.def_property_readonly(
				"position",
				synchronized(
					[]( const Atom & p_atom )
					{
						getTopology( p_atom.entity );
						return Helper::System::getAtomPosition( p_atom.entity, p_atom.index );
					}
				)
			)
			.def( "getSystem", synchronized( &Atom::getSystem ) )
			.def( "getResidue", synchronized( &Atom::getResidue ) )
			.def( "getChain", synchronized( &Atom::getChain ) )
			.def( "getCategory", synchronized( &Atom::getCategory ) );
	}
} // namespace VTX::App::PythonBinding::Topology
