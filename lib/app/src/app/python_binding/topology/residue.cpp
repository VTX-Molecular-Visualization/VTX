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
	{
		return { entity, toUnderlying( getTopology( entity ).getResidueCategory( index ) ) };
	}

	AtomCollection Residue::getAtoms() const
	{
		return { entity, RangeList( getTopology( entity ).getResidueAtomRange( index ) ) };
	}

	Atom Residue::getAtom( const Index p_index ) const
	{
		validateResidueAtomLocalRange( *this, Range( p_index ) );
		return { entity, static_cast<Index>( getTopology( entity ).getResidueFirstAtom( index ) + p_index ) };
	}

	void bindResidue( pybind11::module_ & p_module )
	{
		py::class_<Residue>( p_module, "Residue", py::module_local() )
			.def_property_readonly( "index", []( const Residue & p_residue ) { return p_residue.index; } )
			.def_property_readonly(
				"name",
				[]( const Residue & p_residue )
				{ return getTopology( p_residue.entity ).getResidueName( p_residue.index ); }
			)
			.def_property_readonly(
				"symbol",
				[]( const Residue & p_residue )
				{ return getTopology( p_residue.entity ).getResidueSymbol( p_residue.index ); }
			)
			.def_property_readonly(
				"shortName",
				[]( const Residue & p_residue )
				{
					const auto symbol = getTopology( p_residue.entity ).getResidueSymbol( p_residue.index );
					return std::string( Core::ChemDB::Residue::SYMBOL_SHORT_STR[ toUnderlying( symbol ) ] );
				}
			)
			.def_property_readonly(
				"longName",
				[]( const Residue & p_residue )
				{
					const auto symbol = getTopology( p_residue.entity ).getResidueSymbol( p_residue.index );
					return std::string( Core::ChemDB::Residue::SYMBOL_NAME[ toUnderlying( symbol ) ] );
				}
			)
			.def_property_readonly(
				"atomCount",
				[]( const Residue & p_residue )
				{ return getTopology( p_residue.entity ).getResidueAtomCount( p_residue.index ); }
			)
			.def_property_readonly(
				"indexFirstAtom",
				[]( const Residue & p_residue )
				{ return getTopology( p_residue.entity ).getResidueFirstAtom( p_residue.index ); }
			)
			.def_property_readonly(
				"indexLastAtom",
				[]( const Residue & p_residue )
				{
					const Range range = getTopology( p_residue.entity ).getResidueAtomRange( p_residue.index );
					return range.getLast();
				}
			)
			.def_property_readonly(
				"originalId",
				[]( const Residue & p_residue )
				{ return getTopology( p_residue.entity ).getResidueOriginalId( p_residue.index ); }
			)
			.def( "getSystem", &Residue::getSystem )
			.def( "getChain", &Residue::getChain )
			.def( "getCategory", &Residue::getCategory )
			.def( "getAtoms", &Residue::getAtoms )
			.def( "getAtom", &Residue::getAtom, py::arg( "index" ) )
			.def(
				"setVisible",
				[]( const Residue & p_residue, const bool p_visible )
				{ setVisible<SystemItem::RESIDUE>( p_residue.entity, RangeList( p_residue.index ), p_visible ); },
				py::arg( "visible" ) = true
			)
			.def(
				"setSelected",
				[]( const Residue & p_residue, const bool p_selected, const bool p_append )
				{
					setSelected<SystemItem::RESIDUE>(
						p_residue.entity, RangeList( p_residue.index ), p_selected, p_append
					);
				},
				py::arg( "selected" ) = true,
				py::arg( "append" )	  = false
			)
			.def_property_readonly(
				"visibleState",
				[]( const Residue & p_residue )
				{ return getVisibleState( p_residue.entity, SystemItem::RESIDUE, p_residue.index ); }
			)
			.def_property_readonly(
				"selectionState",
				[]( const Residue & p_residue )
				{ return getSelectionState( p_residue.entity, SystemItem::RESIDUE, p_residue.index ); }
			)
			.def(
				"isVisible",
				[]( const Residue & p_residue )
				{ return isVisible( p_residue.entity, SystemItem::RESIDUE, p_residue.index ); }
			)
			.def(
				"isFullyVisible",
				[]( const Residue & p_residue )
				{ return isFullyVisible( p_residue.entity, SystemItem::RESIDUE, p_residue.index ); }
			)
			.def(
				"isSelected",
				[]( const Residue & p_residue )
				{ return isSelected( p_residue.entity, SystemItem::RESIDUE, p_residue.index ); }
			)
			.def(
				"isFullySelected",
				[]( const Residue & p_residue )
				{ return isFullySelected( p_residue.entity, SystemItem::RESIDUE, p_residue.index ); }
			);
	}
} // namespace VTX::App::PythonBinding::Topology
