#include <pybind11/pybind11.h>

namespace VTX::PythonBinding
{
	/**
	 * @brief Declare VTX python API into the input module
	 * @param p_apiModule : Module that'll recieve VTX API declarations
	 */
	void apiModule( pybind11::module_ & p_apiModule );
} // namespace VTX::PythonBinding
