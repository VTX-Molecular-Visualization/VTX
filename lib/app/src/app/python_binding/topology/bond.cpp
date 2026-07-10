#include "app/python_binding/topology/binding.hpp"
#include "app/python_binding/topology/helpers.hpp"
#include <core/chemdb/bond.hpp>

namespace VTX::App::PythonBinding::Topology
{
	namespace py = pybind11;

	System Bond::getSystem() const
	{
		getTopology( entity );
		return { entity };
	}

	Atom Bond::getFirstAtom() const { return { entity, getTopology( entity ).getBondFirstAtom( index ) }; }

	Atom Bond::getSecondAtom() const { return { entity, getTopology( entity ).getBondSecondAtom( index ) }; }

	void bindBond( pybind11::module_ & p_module )
	{
		py::class_<Bond>( p_module, "Bond", py::module_local() )
			.def_property_readonly( "index", []( const Bond & p_bond ) { return p_bond.index; } )
			.def( "getSystem", &Bond::getSystem )
			.def( "getFirstAtom", &Bond::getFirstAtom )
			.def( "getSecondAtom", &Bond::getSecondAtom )
			.def_property_readonly(
				"order",
				[]( const Bond & p_bond )
				{
					return Core::ChemDB::Bond::ORDER_STR[ size_t( getTopology( p_bond.entity )
																	  .getBondOrder( p_bond.index ) ) ];
				}
			);
	}
} // namespace VTX::App::PythonBinding::Topology
