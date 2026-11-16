#include "app/python_binding/identity.hpp"
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
	{ return { entity, getTopology( entity ).getCategoryAtomRangeList( index ) }; }

	void bindCategory( pybind11::module_ & p_module )
	{
		bindIdentity(
			bindState<SystemItem::CATEGORY>( bindStyle(
				py::class_<Category>( p_module, "Category", py::module_local() ),
				[]( const Category & p_category )
				{ return Target { p_category.entity, SystemItem::CATEGORY, RangeList( p_category.index ) }; }
			) )
		)
			.def_property_readonly( "index", []( const Category & p_category ) { return p_category.index; } )
			.def_property_readonly(
				"name",
				[]( const Category & p_category )
				{ return std::string( Core::ChemDB::Category::TYPE_STR[ p_category.index ] ); }
			)
			.def_property_readonly(
				"residueCount",
				synchronized( []( const Category & p_category ) { return p_category.getResidues().ranges.size(); } )
			)
			.def_property_readonly(
				"atomCount",
				synchronized( []( const Category & p_category ) { return p_category.getAtoms().ranges.size(); } )
			)
			.def( "getSystem", synchronized( &Category::getSystem ) )
			.def( "getResidues", synchronized( &Category::getResidues ) )
			.def( "getAtoms", synchronized( &Category::getAtoms ) );
	}
} // namespace VTX::App::PythonBinding::Topology
