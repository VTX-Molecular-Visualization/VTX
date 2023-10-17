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

	const std::unique_ptr<PythonBinding::Interpretor> interpretorPtr = std::make_unique<PythonBinding::Interpretor>();
	if ( !App::VTXApp::get().getSystem().exists( PythonBinding::Interpretor::SYSTEM_KEY ) )
		App::VTXApp::get().getSystem().referenceSystem( PythonBinding::Interpretor ::SYSTEM_KEY, interpretorPtr.get() );

	interpretorPtr->addBinder<VTX::PythonBinding::Binding::VTXAppBinder>();
	interpretorPtr->addBinder<VTX::Test::ExternalTool::Binder>();

	interpretorPtr->init();

	PythonBinding::Interpretor & interpretor = *interpretorPtr;

	interpretor.runCommand( "new_command()" );
};
