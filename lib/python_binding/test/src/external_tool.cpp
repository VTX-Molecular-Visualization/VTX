#include "external_tool/binding.hpp"
#include "util/app.hpp"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <python_binding/binding/vtx_app_binder.hpp>
#include <python_binding/interpretor.hpp>

TEST_CASE( "VTX_PYTHON_BINDING - External tool test", "[integration]" )
{
	using namespace VTX;

	App::Test::Util::App::initApp();

	std::unique_ptr<PythonBinding::Interpretor> interpretor = App::Test::Util::App::createInterpretor();

	interpretor->addBinder<VTX::Test::ExternalTool::Binder>();
	interpretor->init();

	interpretor->runCommand( "new_command()" );
};
