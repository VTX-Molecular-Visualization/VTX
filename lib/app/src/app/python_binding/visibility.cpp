#include "app/python_binding/visibility.hpp"
#include "app/action/visibility.hpp"
#include "app/system/visibility.hpp"
#include <pybind11/pybind11.h>
#include <python_binding/binding/helper.hpp>

namespace VTX::App::PythonBinding
{
	void VisibilityBinder::bind( Module & p_vtxModule )
	{
		const pybind11::module_ & module = p_vtxModule.pyModule();

		VTX::PythonBinding::Helper::declareEnum<App::System::E_VISIBLE_STATE>( module, "VISIBLE_STATE" );

		p_vtxModule.def(
			"showAll",
			[]() { executeAction<App::Action::Visibility::SetVisibleAll>( true ); },
			"Show all loaded systems."
		);
		p_vtxModule.def(
			"hideAll",
			[]() { executeAction<App::Action::Visibility::SetVisibleAll>( false ); },
			"Hide all loaded systems."
		);
	}
} // namespace VTX::App::PythonBinding
