#include "app/helper/system.hpp"
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
		py::class_<Atom>( p_module, "Atom", py::module_local() )
			.def_property_readonly( "index", []( const Atom & p_atom ) { return p_atom.index; } )
			.def_property_readonly(
				"name", []( const Atom & p_atom ) { return getTopology( p_atom.entity ).getAtomName( p_atom.index ); }
			)
			.def_property_readonly(
				"symbol",
				[]( const Atom & p_atom ) { return getTopology( p_atom.entity ).getAtomSymbol( p_atom.index ); }
			)
			.def_property_readonly(
				"symbolName",
				[]( const Atom & p_atom )
				{
					const auto symbol = getTopology( p_atom.entity ).getAtomSymbol( p_atom.index );
					return std::string( Core::ChemDB::Atom::SYMBOL_NAME[ toUnderlying( symbol ) ] );
				}
			)
			.def_property_readonly(
				"vdwRadius",
				[]( const Atom & p_atom )
				{
					const auto symbol = getTopology( p_atom.entity ).getAtomSymbol( p_atom.index );
					return Core::ChemDB::Atom::SYMBOL_VDW_RADIUS[ toUnderlying( symbol ) ];
				}
			)
			.def_property_readonly(
				"originalIndex",
				[]( const Atom & p_atom ) { return getTopology( p_atom.entity ).getAtomOriginalIndex( p_atom.index ); }
			)
			.def_property_readonly(
				"position",
				[]( const Atom & p_atom ) { return Helper::System::getAtomPosition( p_atom.entity, p_atom.index ); }
			)
			.def( "getSystem", &Atom::getSystem )
			.def( "getResidue", &Atom::getResidue )
			.def( "getChain", &Atom::getChain )
			.def( "getCategory", &Atom::getCategory )
			.def(
				"setVisible",
				[]( const Atom & p_atom, const bool p_visible )
				{ setVisible<SystemItem::ATOM>( p_atom.entity, RangeList( p_atom.index ), p_visible ); },
				py::arg( "visible" ) = true
			)
			.def(
				"setSelected",
				[]( const Atom & p_atom, const bool p_selected, const bool p_append )
				{ setSelected<SystemItem::ATOM>( p_atom.entity, RangeList( p_atom.index ), p_selected, p_append ); },
				py::arg( "selected" ) = true,
				py::arg( "append" )	  = false
			)
			.def_property_readonly(
				"visibleState",
				[]( const Atom & p_atom ) { return getVisibleState( p_atom.entity, SystemItem::ATOM, p_atom.index ); }
			)
			.def_property_readonly(
				"selectionState",
				[]( const Atom & p_atom ) { return getSelectionState( p_atom.entity, SystemItem::ATOM, p_atom.index ); }
			)
			.def(
				"isVisible",
				[]( const Atom & p_atom ) { return isVisible( p_atom.entity, SystemItem::ATOM, p_atom.index ); }
			)
			.def(
				"isFullyVisible",
				[]( const Atom & p_atom ) { return isFullyVisible( p_atom.entity, SystemItem::ATOM, p_atom.index ); }
			)
			.def(
				"isSelected",
				[]( const Atom & p_atom ) { return isSelected( p_atom.entity, SystemItem::ATOM, p_atom.index ); }
			)
			.def(
				"isFullySelected",
				[]( const Atom & p_atom ) { return isFullySelected( p_atom.entity, SystemItem::ATOM, p_atom.index ); }
			);
	}
} // namespace VTX::App::PythonBinding::Topology
