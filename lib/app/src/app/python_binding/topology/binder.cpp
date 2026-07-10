#include "app/python_binding/topology/binder.hpp"
#include "app/python_binding/topology/binding.hpp"
#include <core/chemdb/atom.hpp>
#include <core/chemdb/residue.hpp>
#include <python_binding/binding/helper.hpp>

namespace VTX::App::PythonBinding
{
	void TopologyBinder::bind( Module & p_vtxModule )
	{
		pybind11::module_ & module = p_vtxModule.pyModule();

		VTX::PythonBinding::Helper::declareEnum<Core::ChemDB::Residue::SYMBOL>( module, "RESIDUE_SYMBOL" );
		VTX::PythonBinding::Helper::declareEnum<Core::ChemDB::Atom::SYMBOL>( module, "ATOM_SYMBOL" );
		VTX::PythonBinding::Helper::declareEnum<Core::ChemDB::Atom::TYPE>( module, "ATOM_TYPE" );

		Topology::bindCollections( module );
		Topology::bindSystem( p_vtxModule, module );
		Topology::bindChain( module );
		Topology::bindResidue( module );
		Topology::bindAtom( module );
		Topology::bindBond( module );
		Topology::bindCategory( module );
	}
} // namespace VTX::App::PythonBinding
