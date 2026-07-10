#include "app/python_binding/topology/actions.hpp"
#include "app/python_binding/topology/binding.hpp"
#include "app/python_binding/topology/helpers.hpp"
#include <core/chemdb/category.hpp>

namespace VTX::App::PythonBinding::Topology
{
	namespace py = pybind11;

	System Category::getSystem() const
	{
		getTopology( entity );
		return { entity };
	}

	ResidueCollection Category::getResidues() const
	{
		RangeList ranges;
		for ( const Index residue :
			  getTopology( entity ).getCategoryResidues( static_cast<Core::ChemDB::Category::TYPE>( index ) ) )
		{
			ranges.addValue( residue );
		}
		return { entity, ranges };
	}

	AtomCollection Category::getAtoms() const
	{
		return { entity, getTopology( entity ).getCategoryAtomRangeList( index ) };
	}

	void bindCategory( pybind11::module_ & p_module )
	{
		py::class_<Category>( p_module, "Category", py::module_local() )
			.def_property_readonly( "index", []( const Category & p_category ) { return p_category.index; } )
			.def_property_readonly(
				"name",
				[]( const Category & p_category )
				{ return std::string( Core::ChemDB::Category::TYPE_STR[ p_category.index ] ); }
			)
			.def_property_readonly(
				"residueCount", []( const Category & p_category ) { return p_category.getResidues().ranges.size(); }
			)
			.def_property_readonly(
				"atomCount", []( const Category & p_category ) { return p_category.getAtoms().ranges.size(); }
			)
			.def( "getSystem", &Category::getSystem )
			.def( "getResidues", &Category::getResidues )
			.def( "getAtoms", &Category::getAtoms )
			.def(
				"setVisible",
				[]( const Category & p_category, const bool p_visible )
				{ setVisible<SystemItem::CATEGORY>( p_category.entity, RangeList( p_category.index ), p_visible ); },
				py::arg( "visible" ) = true
			)
			.def(
				"setSelected",
				[]( const Category & p_category, const bool p_selected, const bool p_append )
				{
					setSelected<SystemItem::CATEGORY>(
						p_category.entity, RangeList( p_category.index ), p_selected, p_append
					);
				},
				py::arg( "selected" ) = true,
				py::arg( "append" )	  = false
			)
			.def_property_readonly(
				"visibleState",
				[]( const Category & p_category )
				{ return getVisibleState( p_category.entity, SystemItem::CATEGORY, p_category.index ); }
			)
			.def_property_readonly(
				"selectionState",
				[]( const Category & p_category )
				{ return getSelectionState( p_category.entity, SystemItem::CATEGORY, p_category.index ); }
			)
			.def(
				"isVisible",
				[]( const Category & p_category )
				{ return isVisible( p_category.entity, SystemItem::CATEGORY, p_category.index ); }
			)
			.def(
				"isFullyVisible",
				[]( const Category & p_category )
				{ return isFullyVisible( p_category.entity, SystemItem::CATEGORY, p_category.index ); }
			)
			.def(
				"isSelected",
				[]( const Category & p_category )
				{ return isSelected( p_category.entity, SystemItem::CATEGORY, p_category.index ); }
			)
			.def(
				"isFullySelected",
				[]( const Category & p_category )
				{ return isFullySelected( p_category.entity, SystemItem::CATEGORY, p_category.index ); }
			);
	}
} // namespace VTX::App::PythonBinding::Topology
