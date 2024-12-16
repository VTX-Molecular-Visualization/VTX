#include "python_binding/binding/binders/selection.hpp"
#include "python_binding/api/selection/selection_interpretor.hpp"
#include "python_binding/api/selection/selection_wrapper.hpp"
#include "python_binding/api/selection/system_interpretor.hpp"
#include <app/application/selection/selection.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/component/chemistry/system.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <string>
#include <util/logger.hpp>
#include <vector>

namespace VTX::PythonBinding::Binding::Binders
{
	// TMP
	void justForLinkingFunction()
	{
		API::Selection::SelectionInterpretor::clear();
		API::Selection::SelectionInterpretor::addInterpretor( &API::Selection::SystemInterpretor::interpretSystems );
		VTX::VTX_WARNING( "SHOULD NEVER BE CALLED AAAAAAH" );
	}
	//! TMP

	void bind_selection( pybind11::module_ & p_apiModule )
	{
		using namespace VTX::App;

		// TMP - We try to bamblooze compiler into never calling justForLinkingFunction at runtime, but still having it
		// compiled.
		if ( reinterpret_cast<uint64_t>( &p_apiModule ) == 0 )
			justForLinkingFunction();
		//! TMP
		/* TMP
		API::Selection::SelectionInterpretor::clear();
		API::Selection::SelectionInterpretor::addInterpretor( &API::Selection::SystemInterpretor::interpretSystems );

		pybind11::bind_vector<std::vector<Component::Chemistry::System *>>( p_apiModule, "MolPtrVector" );
		pybind11::bind_vector<std::vector<Component::Chemistry::Chain *>>( p_apiModule, "ChainPtrVector" );
		pybind11::bind_vector<std::vector<Component::Chemistry::Residue *>>( p_apiModule, "ResPtrVector" );
		pybind11::bind_vector<std::vector<Component::Chemistry::Atom *>>( p_apiModule, "AtomPtrVector" );

		p_apiModule.def(
			"select", &API::Selection::SelectionInterpretor::select, pybind11::return_value_policy::reference
		);

		p_apiModule.def( "intersect", &API::Selection::SelectionWrapper::intersect );
		p_apiModule.def( "exclusive", &API::Selection::SelectionWrapper::exclusive );

		pybind11::class_<App::Application::Selection::Selection>(
			p_apiModule, "_VTXSelection", pybind11::module_local()
		);

		pybind11::class_<API::Selection::SelectionWrapper>( p_apiModule, "Selection", pybind11::module_local() )
			.def( "__str__", &API::Selection::SelectionWrapper::toString )
			.def( "__add__", &API::Selection::SelectionWrapper::add, pybind11::return_value_policy::reference )
			.def( "__sub__", &API::Selection::SelectionWrapper::remove, pybind11::return_value_policy::reference )
			.def( "save", &API::Selection::SelectionWrapper::save )
			.def( "getSystems", &API::Selection::SelectionWrapper::getSystems )
			.def( "getChains", &API::Selection::SelectionWrapper::getChains )
			.def( "getResidues", &API::Selection::SelectionWrapper::getResidues )
			.def( "getAtoms", &API::Selection::SelectionWrapper::getAtoms );
		TMP */
	}
} // namespace VTX::PythonBinding::Binding::Binders
