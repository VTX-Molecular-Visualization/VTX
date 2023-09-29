#include "python_binding/interpretor.hpp"
// #include <app/application/ecs/registry_manager.hpp>
#include <app/vtx_app.hpp>
#include <io/internal/filesystem.hpp>
#include <pybind11/embed.h>
#include <util/exceptions.hpp>
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
		try
		{
			pybind11::exec( "import PyTX" );
			pybind11::exec( p_line );
		}
		catch ( pybind11::error_already_set & error )
		{
			VTX_ERROR( "Error when running command {} : {}", p_line, error.what() );
			throw( VTX::CommandException( p_line, error.what() ) );
		}

		// const int result = _impl->vtxModule().attr( "test" )( 5 ).cast<int>();
		// VTX_INFO( "Run command {} with result {}", p_line, result );
	}
	void Interpretor::runTest()
	{
		try
		{
			const int result = _impl->vtxModule().attr( "test" )( 5 ).cast<int>();
			VTX_INFO( "Run command test with result {}", result );
		}
		catch ( pybind11::error_already_set & error )
		{
			VTX_ERROR( "Error when running command {} : {}", "test", error.what() );
			return;
		}

		const FilePath path = VTX::IO::Internal::Filesystem::getInternalDataDir() / "1AGA.mmtf";

		try
		{
			_impl->vtxModule().attr( "openFile" )( App::VTXApp::get().getSystemPtr(), path.string() );
		}
		catch ( pybind11::error_already_set & error )
		{
			VTX_ERROR( "Error when running command {} : {}", "openFile", error.what() );
			return;
		}
	}

} // namespace VTX::PythonBinding
