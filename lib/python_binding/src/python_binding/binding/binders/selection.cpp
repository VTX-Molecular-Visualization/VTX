#include "python_binding/binding/binders/selection.hpp"
#include "python_binding/api/selection/molecule_interpretor.hpp"
#include "python_binding/api/selection_wrapper.hpp"
#include <app/application/selection/selection.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/chemistry/residue.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <string>
#include <vector>

namespace VTX::PythonBinding::Binding::Binders
{
	void bind_selection( pybind11::module_ & p_apiModule )
	{
		using namespace VTX::App;

		API::Selection::SelectionWrapper::addInterpretor( &API::Selection::MoleculeInterpretor::interpretMolecules );

		pybind11::bind_vector<std::vector<Component::Chemistry::Molecule *>>( p_apiModule, "MolPtrVector" );
		pybind11::bind_vector<std::vector<Component::Chemistry::Chain *>>( p_apiModule, "ChainPtrVector" );
		pybind11::bind_vector<std::vector<Component::Chemistry::Residue *>>( p_apiModule, "ResPtrVector" );
		pybind11::bind_vector<std::vector<Component::Chemistry::Atom *>>( p_apiModule, "AtomPtrVector" );

		p_apiModule.def(
			"select", &API::Selection::SelectionWrapper::select, pybind11::return_value_policy::reference
		);

		pybind11::class_<App::Application::Selection::Selection>(
			p_apiModule, "_VTXSelection", pybind11::module_local()
		);

		pybind11::class_<API::Selection::SelectionWrapper>( p_apiModule, "Selection", pybind11::module_local() )
			.def( "__str__", &API::Selection::SelectionWrapper::toString )
			.def( "__add__", &API::Selection::SelectionWrapper::add )
			.def( "getMolecules", &API::Selection::SelectionWrapper::getMolecules )
			.def( "getMolecules", &API::Selection::SelectionWrapper::getMolecules )
			.def( "getChains", &API::Selection::SelectionWrapper::getChains )
			.def( "getResidues", &API::Selection::SelectionWrapper::getResidues )
			.def( "getAtoms", &API::Selection::SelectionWrapper::getAtoms );
	}
} // namespace VTX::PythonBinding::Binding::Binders
