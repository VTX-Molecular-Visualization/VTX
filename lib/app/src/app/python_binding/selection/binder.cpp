#include "app/python_binding/selection/binder.hpp"
#include "app/python_binding/selection/selection_interpretor.hpp"
#include "app/python_binding/selection/selection_wrapper.hpp"
#include "app/python_binding/selection/system_interpretor.hpp"
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/component/chemistry/system.hpp>
#include <app/selection/selection.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <string>
#include <util/logger.hpp>
#include <vector>

namespace VTX::App::PythonBinding::Selection
{

	void bind_selection( pybind11::module_ & p_apiModule )
	{
		using namespace VTX::App;

		VTX::App::PythonBinding::Selection::SelectionInterpretor::clear();
		VTX::App::PythonBinding::Selection::SelectionInterpretor::addInterpretor(
			&VTX::App::PythonBinding::Selection::SystemInterpretor::interpretSystems
		);

		pybind11::bind_vector<std::vector<Component::Chemistry::System *>>( p_apiModule, "MolPtrVector" );
		pybind11::bind_vector<std::vector<Component::Chemistry::Chain *>>( p_apiModule, "ChainPtrVector" );
		pybind11::bind_vector<std::vector<Component::Chemistry::Residue *>>( p_apiModule, "ResPtrVector" );
		pybind11::bind_vector<std::vector<Component::Chemistry::Atom *>>( p_apiModule, "AtomPtrVector" );

		p_apiModule.def(
			"select",
			&VTX::App::PythonBinding::Selection::SelectionInterpretor::select,
			pybind11::return_value_policy::reference
		);

		p_apiModule.def( "intersect", &VTX::App::PythonBinding::Selection::SelectionWrapper::intersect );
		p_apiModule.def( "exclusive", &VTX::App::PythonBinding::Selection::SelectionWrapper::exclusive );

		pybind11::class_<App::Selection::Selection>( p_apiModule, "_VTXSelection", pybind11::module_local() );

		pybind11::class_<VTX::App::PythonBinding::Selection::SelectionWrapper>(
			p_apiModule, "Selection", pybind11::module_local()
		)
			.def( "__str__", &VTX::App::PythonBinding::Selection::SelectionWrapper::toString )
			.def(
				"__add__",
				&VTX::App::PythonBinding::Selection::SelectionWrapper::add,
				pybind11::return_value_policy::reference
			)
			.def(
				"__sub__",
				&VTX::App::PythonBinding::Selection::SelectionWrapper::remove,
				pybind11::return_value_policy::reference
			)
			.def( "save", &VTX::App::PythonBinding::Selection::SelectionWrapper::save )
			.def( "getSystems", &VTX::App::PythonBinding::Selection::SelectionWrapper::getSystems )
			.def( "getChains", &VTX::App::PythonBinding::Selection::SelectionWrapper::getChains )
			.def( "getResidues", &VTX::App::PythonBinding::Selection::SelectionWrapper::getResidues )
			.def( "getAtoms", &VTX::App::PythonBinding::Selection::SelectionWrapper::getAtoms );
	}
} // namespace VTX::App::PythonBinding::Selection
