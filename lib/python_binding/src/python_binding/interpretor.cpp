#include "python_binding/interpretor.hpp"
#include "python_binding/binder.hpp"
#include "python_binding/log_redirection.hpp"
#include "python_binding/vtx_python_module.hpp"
#include "python_binding/wrapper/module.hpp"
#include <app/vtx_app.hpp>
#include <io/internal/filesystem.hpp>
#include <pybind11/embed.h>
#include <util/exceptions.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

namespace VTX::PythonBinding
{
	struct Interpretor::Impl
	{
	  public:
		void initializePythonModule()
		{
			_vtxModule = pybind11::module_::import( "vtx_python_bin" );

			LogRedirection logger								= LogRedirection();
			pybind11::module_::import( "sys" ).attr( "stdout" ) = logger;

			pybind11::module_ vtxCoreModule = pybind11::module_::import( "vtx_python_bin.Core" );
			// vtxCoreModule.attr( "_init" )( APP::getSystemHandlerPtr() );

			FilePath initScriptDir	  = Util::Filesystem::getExecutableDir() / "python_script";
			FilePath initCommandsFile = initScriptDir / "pytx_init.py";

			pybind11::eval_file( initCommandsFile.string() );
		}

		void addBinder( std::unique_ptr<Binder> p_binder ) { _binders.emplace_back( std::move( p_binder ) ); }

		void applyBinders()
		{
			Wrapper::Module moduleWrapper = Wrapper::Module( _vtxModule, "vtx_python_bin" );
			_pyTXModule					  = std::make_unique<PyTXModule>( moduleWrapper );

			for ( const std::unique_ptr<Binder> & binder : _binders )
			{
				binder->bind( *_pyTXModule );
			}
		}

		void importCommands()
		{
			// Import all commands
			pybind11::exec( "from vtx_python_bin.Command import *" );

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
			VTX_ERROR( "{}", e.what() );
			throw e;
		}
	}
	Interpretor::~Interpretor() { VTX_INFO( "Destructing interpretor ..." ); }

	void Interpretor::init()
	{
		try
		{
			_impl->applyBinders();
			_impl->importCommands();
		}
		catch ( const std::exception & e )
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
