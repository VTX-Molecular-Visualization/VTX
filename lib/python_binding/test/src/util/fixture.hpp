#pragma once

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
