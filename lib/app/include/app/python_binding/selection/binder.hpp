#ifndef __VTX_APP_PYTHON_BINDING_SELECTION_BINDER__
#define __VTX_APP_PYTHON_BINDING_SELECTION_BINDER__

namespace pybind11
{
	class module_;
}

namespace VTX::App::PythonBinding::Selection
{
	/**
	 * @brief Bind selection actions to commands in the input module.
	 * @param p_apiModule : Module which is going to receive commands
	 */
	void bind_selection( pybind11::module_ & p_apiModule );
} // namespace VTX::App::PythonBinding::Selection

#endif // !__VTX_APP_PYTHON_BINDING_SELECTION_BINDER__
