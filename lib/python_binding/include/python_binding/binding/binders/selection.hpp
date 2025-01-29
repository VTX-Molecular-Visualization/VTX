#include <pybind11/pybind11.h>

namespace VTX::PythonBinding::Binding::Binders
{
	/**
	 * @brief Bind selection actions to commands in the input module.
	 * @param p_apiModule : Module which is going to receive commands
	 */
	void bind_selection( pybind11::module_ & p_apiModule );
} // namespace VTX::PythonBinding::Binding::Binders
