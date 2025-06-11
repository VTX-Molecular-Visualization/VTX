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
#include <python_binding/api/api.hpp>
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

		p_apiModule.def(
			"select",
			[]( const pybind11::kwargs & kw )
			{
				//
				return VTX::App::PythonBinding::Selection::SelectionInterpretor::select( kw );
				//
			},
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

			// I don't like the 4 following definition but the "implicitly_convertible" pybind function doesn't work
			// here and is seems that having the method signature conversion makes python garbage the memory while we
			// use it.
			.def(
				"getSystems",
				[]( VTX::App::PythonBinding::Selection::SelectionWrapper & _ )
				{ return VTX::PythonBinding::API::Collection<VTX::PythonBinding::API::System> { _.getSystems() }; }
			)
			.def(
				"getChains",
				[]( VTX::App::PythonBinding::Selection::SelectionWrapper & _ )
				{ return VTX::PythonBinding::API::Collection<VTX::PythonBinding::API::Chain>( _.getChains() ); }
			)
			.def(
				"getResidues",
				[]( VTX::App::PythonBinding::Selection::SelectionWrapper & _ )
				{ return VTX::PythonBinding::API::Collection<VTX::PythonBinding::API::Residue>( _.getResidues() ); }
			)
			.def(
				"getAtoms",
				[]( VTX::App::PythonBinding::Selection::SelectionWrapper & _ )
				{ return VTX::PythonBinding::API::Collection<VTX::PythonBinding::API::Atom>( _.getAtoms() ); }
			)
			//.def( "getAtoms", &VTX::App::PythonBinding::Selection::SelectionWrapper::getAtoms )
			//.def(
			//	"getSystems",
			//	(VTX::PythonBinding::API::Collection<
			//		VTX::PythonBinding::API::System> ( VTX::App::PythonBinding::Selection::SelectionWrapper::* )()
			//	)&VTX::App::PythonBinding::Selection::SelectionWrapper::getSystems,
			//	pybind11::return_value_policy::take_ownership
			//)
			//.def(
			//	"getChains",
			//	(VTX::PythonBinding::API::Collection<
			//		VTX::PythonBinding::API::Chain> ( VTX::App::PythonBinding::Selection::SelectionWrapper ::* )()
			//	)&VTX::App::PythonBinding::Selection::SelectionWrapper::getChains,
			//	pybind11::return_value_policy::take_ownership
			//)
			//.def(
			//	"getResidues",
			//	(VTX::PythonBinding::API::Collection<
			//		VTX::PythonBinding::API::Residue> ( VTX::App::PythonBinding::Selection::SelectionWrapper::* )()
			//	)&VTX::App::PythonBinding::Selection::SelectionWrapper::getResidues,
			//	pybind11::return_value_policy::take_ownership
			//)
			//.def(
			//	"getAtoms",
			//	(VTX::PythonBinding::API::Collection<
			//		VTX::PythonBinding::API::Atom> ( VTX::App::PythonBinding::Selection::SelectionWrapper::* )()
			//	)&VTX::App::PythonBinding::Selection::SelectionWrapper::getAtoms,
			//	pybind11::return_value_policy::move
			//)

			;
	}
} // namespace VTX::App::PythonBinding::Selection
