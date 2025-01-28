#include "python_binding/interpretor.hpp"
#include "python_binding/binder.hpp"
#include "python_binding/binding/vtx_module.hpp"
#include "python_binding/log_redirection.hpp"
#include "python_binding/vtx_python_module.hpp"
#include "python_binding/wrapper/module.hpp"
#include <app/vtx_app.hpp>
#include <io/internal/filesystem.hpp>
#include <pybind11/embed.h>
#include <source_location>
#include <util/exceptions.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

namespace VTX
{
	// PythonBinding::Interpretor * g_TMP = nullptr;
	Util::Singleton<PythonBinding::Interpretor> g_interpretor;
	PythonBinding::Interpretor &				INTERPRETOR() { return g_interpretor.get(); }
	void										INTERPRETOR( PythonBinding::Interpretor * _ ) { /*g_TMP = _;*/ }
} // namespace VTX

namespace VTX::PythonBinding
{

	struct Interpretor::Impl
	{
	  public:
		void initializePythonModule()
		{
			VTX::VTX_INFO( "Importing python module <{}>", vtx_module_name() );
			_vtxModule = pybind11::module_::import( vtx_module_name() );

			// Allow the python "print" function to be funneled into our log system
			_vtxModule.import( "sys" ).attr( "stdout" ) = _vtxModule.attr( "LogRedirection" );

			pybind11::module_ vtxCoreModule
				= pybind11::module_::import( ( std::string( vtx_module_name() ) + ".Core" ).c_str() );
			// vtxCoreModule.attr( "_init" )( APP::getSystemHandlerPtr() );

			// TODO : Manage case where file not found (e.g. user moved it elsewhere)
			FilePath initScriptDir	  = Util::Filesystem::getExecutableDir() / "python_script";
			FilePath initCommandsFile = initScriptDir / vtx_initialization_script_name();

			pybind11::eval_file( initCommandsFile.string() );
		}

		void addBinder( std::unique_ptr<Binder> p_binder ) { _binders.emplace_back( std::move( p_binder ) ); }

		void applyBinders()
		{
			Wrapper::Module moduleWrapper = Wrapper::Module( _vtxModule, vtx_module_name() );
			_pyTXModule					  = std::make_unique<PyTXModule>( moduleWrapper );

			for ( const std::unique_ptr<Binder> & binder : _binders )
			{
				binder->bind( *_pyTXModule );
			}
		}

		void importCommands()
		{
			//  Import all commands
			pybind11::exec( fmt::format( "from {}.Command import *", vtx_module_name() ) );

			// Specific imports by binders
			for ( const std::unique_ptr<Binder> & binder : _binders )
			{
				binder->importHeaders();
			}
		}

		PyTXModule & getPyTXModule() { return *_pyTXModule; }

		void clearBinders()
		{
			_binders.clear();
			_binders.shrink_to_fit();
		}

	  private:
		LogRedirection				 _logger;
		pybind11::scoped_interpreter _interpretor {};
		pybind11::module_			 _vtxModule;
		std::unique_ptr<PyTXModule>	 _pyTXModule = nullptr;

		std::vector<std::unique_ptr<Binder>> _binders = std::vector<std::unique_ptr<Binder>>();
	};

	Interpretor::Interpretor() : _impl( std::make_unique<Interpretor::Impl>() )
	{
		try
		{
			_impl->initializePythonModule();
		}
		catch ( const std::exception & e )
		{
			VTX_ERROR( "{} at {}:", e.what(), std::source_location().file_name(), std::source_location().line() );
			throw e;
		}
	}
	Interpretor::~Interpretor()
	{
		VTX_INFO( "Destroying interpreter ..." );
		_impl.reset();
		VTX_INFO( "interpreter destroyed ..." );
	}

	void Interpretor::init()
	{
		try
		{
			_impl->applyBinders();
			_impl->importCommands();
		}
		catch ( std::exception & e )
		{
			VTX_ERROR( "{}", e.what() );
			throw e;
		}
	}

	void Interpretor::addBinder( std::unique_ptr<Binder> p_binder ) { _impl->addBinder( std::move( p_binder ) ); }
	void Interpretor::clearBinders() { _impl->clearBinders(); }

	void Interpretor::runCommand( const std::string & p_line ) const
	{
		try
		{
			VTX_DEBUG( "Run Command : {}", p_line );
			pybind11::exec( p_line );
		}
		catch ( const pybind11::error_already_set & e )
		{
			throw( VTX::CommandException( p_line, e.what() ) );
		}
	}
	void Interpretor::runScript( const FilePath & p_path ) const
	{
		try
		{
			// The following line's purpose is to force pybind11 to set the __file__ variable to the path of the new
			// script being used.
			if ( pybind11::globals().contains( "__file__" ) )
				pybind11::globals().attr( "pop" )( "__file__" );

			pybind11::eval_file( p_path.string() );
		}
		catch ( const pybind11::error_already_set & e )
		{
			throw( VTX::ScriptException( p_path.filename().string(), e.what() ) );
		}
	}
	Wrapper::Module Interpretor::loadModule( const FilePath & p_path ) const
	{
		runScript( p_path );

		const std::string dir = p_path.parent_path().string();
		getModule().core().runFunction<void>( "addSysPath", dir );

		const std::string filename = Util::Filesystem::getFileName( p_path );
		pybind11::module  loadedModule;
		try
		{
			loadedModule = pybind11::module_::import( filename.c_str() );
		}
		catch ( const pybind11::error_already_set & e )
		{
			VTX_ERROR( "{}", e.what() );
			throw( e );
		}

		return Wrapper::Module( loadedModule, filename );

	} // namespace VTX::PythonBinding

	const PyTXModule & Interpretor::getModule() const { return _impl->getPyTXModule(); }

	void Interpretor::print( const std::string & p_line ) const { pybind11::print( p_line ); }

} // namespace VTX::PythonBinding
