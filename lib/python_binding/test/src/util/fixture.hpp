#ifndef VTX_PYTHON_BINDING_TEST_UTIL_FIXTURE_HPP
#define VTX_PYTHON_BINDING_TEST_UTIL_FIXTURE_HPP

#include <python_binding/interpretor.hpp>

namespace VTX::PythonBinding::Test
{

	class Fixture
	{
	  public:
		Fixture();

		Interpretor interpretor;
	};
} // namespace VTX::PythonBinding::Test

#endif
