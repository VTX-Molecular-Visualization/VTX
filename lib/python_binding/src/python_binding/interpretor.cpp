#include "python_binding/interpretor.hpp"
// #include <app/application/ecs/registry_manager.hpp>
#include <app/vtx_app.hpp>
#include <pybind11/embed.h>
#include <util/logger.hpp>

namespace VTX::PythonBinding
{
	struct Interpretor::Impl
	{
	  public:
		pybind11::module_ & vtxModule()
		{
			if ( !_vtxModule )
			{
				_vtxModule = pybind11::module_::import( "PyTX" );
			}

			return _vtxModule;
		}

	  private:
		pybind11::scoped_interpreter _interpretor {};
		pybind11::module_			 _vtxModule;
	};

	Interpretor::Interpretor() : _impl( std::make_unique<Interpretor::Impl>() ) {}
	Interpretor::~Interpretor() {}

	void Interpretor::print( const std::string & p_line ) { pybind11::print( p_line ); }

	void Interpretor::runCommand( const std::string & p_line )
	{
		pybind11::exec( "import PyTX" );
		pybind11::exec( p_line );
		// const int result = _impl->vtxModule().attr( "test" )( 5 ).cast<int>();
		// VTX_INFO( "Run command {} with result {}", p_line, result );
	}
	void Interpretor::runTest()
	{
		const int result = _impl->vtxModule().attr( "test" )( 5 ).cast<int>();
		VTX_INFO( "Run command test with result {}", result );

		const std::string path
			= "C:\\Users\\gcpas\\Documents\\VTX\\repositories\\VTX_dev-ndy\\out\\build\\x64-Debug\\lib\\python_"
			  "binding\\data\\1AGA.mmtf";
		_impl->vtxModule().attr( "openFile" )( App::VTXApp::get().getSystemPtr(), path );
	}

} // namespace VTX::PythonBinding
