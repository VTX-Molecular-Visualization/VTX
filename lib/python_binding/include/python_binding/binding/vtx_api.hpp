#include <pybind11/pybind11.h>

namespace VTX::PythonBinding::Binding
{
	/**
	 * @brief Declare VTX python API into the input module. This step is advised if one plan to return VTX type from a
	 * custom module.
	 * @param p_apiModule : Module that'll recieve VTX API declarations
	 */
	void applyVtxApiBinding( pybind11::module_ & p_apiModule );

	/**
	 * @brief Declare VTX python command that are meant to be used only in the desktop client api, not in offline mode
	 * @param p_apiModule
	 */
	void applyVtxLocalCommandBinding( pybind11::module_ & p_apiModule );
} // namespace VTX::PythonBinding::Binding
