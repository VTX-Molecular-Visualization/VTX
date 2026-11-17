#include "app/python_binding/identity.hpp"
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
		bindIdentity( py::class_<Bond>( p_module, "Bond", "Handle to a bond of a system.", py::module_local() ) )
			.def_property_readonly(
				"index", []( const Bond & p_bond ) { return p_bond.index; }, "Index in the system."
			)
			.def( "getSystem", synchronized( &Bond::getSystem ), "Return the parent system." )
			.def( "getFirstAtom", synchronized( &Bond::getFirstAtom ), "Return the first atom." )
			.def( "getSecondAtom", synchronized( &Bond::getSecondAtom ), "Return the second atom." )
			.def_property_readonly(
				"order",
				synchronized(
					[]( const Bond & p_bond )
					{
						return Core::ChemDB::Bond::ORDER_STR[ static_cast<size_t>(
							getTopology( p_bond.entity ).getBondOrder( p_bond.index )
						) ];
					}
				),
				"Bond order."
			);
	}
} // namespace VTX::App::PythonBinding::Topology
