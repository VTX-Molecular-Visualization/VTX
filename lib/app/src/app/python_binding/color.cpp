#include "app/python_binding/color.hpp"
#include <pybind11/pybind11.h>
#include <python_binding/binding/helper.hpp>
#include <renderer/color.hpp>

namespace VTX::App::PythonBinding
{
	void ColorBinder::bind( Module & p_vtxModule )
	{
		const pybind11::module_ & module = p_vtxModule.pyModule();

		VTX::PythonBinding::Helper::declareEnum<Renderer::E_COLOR_SCHEME>( module, "COLOR_SCHEME", "Color scheme." );
		VTX::PythonBinding::Helper::declareEnum<Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE>(
			module, "SECONDARY_STRUCTURE_COLOR_SCHEME", "Secondary structure color scheme."
		);
	}
} // namespace VTX::App::PythonBinding
