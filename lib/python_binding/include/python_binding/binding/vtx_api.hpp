#include <pybind11/pybind11.h>

namespace VTX::PythonBinding::Binding
{
	/**
	 * @brief Declare VTX python API into the input module. This step is advised if one plan to return VTX type from a
	 * custom module.
	 * @param p_apiModule : Module that'll recieve VTX API declarations
	 */
	void applyVtxBinding( pybind11::module_ & p_apiModule );
} // namespace VTX::PythonBinding::Binding
