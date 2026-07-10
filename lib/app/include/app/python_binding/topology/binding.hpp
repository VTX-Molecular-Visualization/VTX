#ifndef __VTX_APP_PYTHON_BINDING_TOPOLOGY_BINDING__
#define __VTX_APP_PYTHON_BINDING_TOPOLOGY_BINDING__

#include "app/python_binding/module.hpp"
#include <pybind11/pybind11.h>

namespace VTX::App::PythonBinding::Topology
{
	void bindCollections( pybind11::module_ & p_module );
	void bindSystem( Module & p_vtxModule, pybind11::module_ & p_module );
	void bindChain( pybind11::module_ & p_module );
	void bindResidue( pybind11::module_ & p_module );
	void bindAtom( pybind11::module_ & p_module );
	void bindBond( pybind11::module_ & p_module );
	void bindCategory( pybind11::module_ & p_module );
} // namespace VTX::App::PythonBinding::Topology

#endif
