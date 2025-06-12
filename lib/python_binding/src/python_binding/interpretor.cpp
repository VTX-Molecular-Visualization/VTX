#include "interpretor.hpp"
#include "python_binding/interpretor.hpp"
#include "python_binding/binder.hpp"
#include "python_binding/binding/vtx_api.hpp"
#include "python_binding/binding/vtx_module.hpp"
#include "python_binding/command_filter.hpp"
#include "python_binding/log_redirection.hpp"
#include "python_binding/vtx_python_module.hpp"
#include "python_binding/wrapper/module.hpp"
#include <io/internal/filesystem.hpp>
#include <pybind11/embed.h>
#include <pybind11/eval.h>
#include <source_location>
#include <util/exceptions.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

namespace VTX::PythonBinding
{

	struct Interpretor::Impl
	{
	  public:
		Impl()
		{
			VTX::VTX_INFO( "Importing python module <{}>", vtx_module_name() );

			// Allow the python "print" function to be funneled into our log system
			_vtxModule.import( "sys" ).attr( "stdout" ) = _vtxModule.attr( "LogRedirection" );

			pybind11::module_ vtxCoreModule
				= pybind11::module_::import( ( std::string( vtx_module_name() ) + ".Core" ).c_str() );
			pybind11::module_ vtxApiModule
				= pybind11::module_::import( ( std::string( vtx_module_name() ) + ".API" ).c_str() );
			Binding::applyVtxApiBinding( _vtxModule );
			pybind11::module_ vtxCommandModule
				= pybind11::module_::import( ( std::string( vtx_module_name() ) + ".Command" ).c_str() );
			Binding::applyVtxLocalCommandBinding( vtxCommandModule );

			FilePath initScriptDir	  = Util::Filesystem::getExecutableDir() / "python_script";
			FilePath initCommandsFile = initScriptDir / vtx_initialization_script_name();

			// The file should be at the right place but users always find a way ...
			if ( not std::filesystem::exists( initCommandsFile ) )
				throw VTX::IOException( "Required file {} not found.", initCommandsFile.string() );
			pybind11::eval_file( initCommandsFile.string() );
		}
		void add( Binder p_binder )
		{
			_binders.push_back( std::move( p_binder ) );
			_binders.back().bind( *_pyTXModule );
			_binders.back().importHeaders();

			// Put newly added command to the module global namespace
			pybind11::exec( fmt::format( "from {}.Command import *", vtx_module_name() ) );
		}

		PyTXModule & getPyTXModule() { return *_pyTXModule; }

		void clearBinders()
		{
			_binders.clear();
			_binders.shrink_to_fit();
		}
		void getPythonModule( pybind11::module_ ** p_modulePtr ) { *p_modulePtr = &_vtxModule; }

	  private:
		LogRedirection				 _logger;
		pybind11::scoped_interpreter _interpretor {};
		pybind11::module_			 _vtxModule { pybind11::module_::import( vtx_module_name() ) };

		std::unique_ptr<PyTXModule> _pyTXModule
			= std::make_unique<PyTXModule>( Wrapper::Module( _vtxModule, vtx_module_name() ) );

		std::vector<Binder> _binders;
	};

	Interpretor::Interpretor() : _impl( std::make_unique<Interpretor::Impl>() ) {}
	Interpretor::~Interpretor() { _impl.reset(); }

	void Interpretor::add( Binder p_binder ) { _impl->add( std::move( p_binder ) ); }

	void Interpretor::clearBinders() { _impl->clearBinders(); }

	std::string Interpretor::runCommand( const std::string & p_line ) const
	{
		// The idea is to try to execute the command as if we expected a return value. If an exception is thrown, then
		// it might mean that we shouldn't expect a return value. So we execute it as is. If it cashes again, it means
		// that the command isn't viable at all.

		if ( FilterResult isHarmful = filter( p_line ) )
			return isHarmful.why();

		try
		{
			VTX_DEBUG( "Run Python Command : {}", p_line );
			return "paf";
			auto result = pybind11::eval<pybind11::eval_expr>( p_line );
			if ( not result.is_none() )
				return result.attr( "__repr__" )().cast<std::string>();
		}
		catch ( const pybind11::error_already_set & )
		{
			try
			{
				pybind11::exec( p_line );
			}
			catch ( const pybind11::error_already_set & ee )
			{
				throw( VTX::CommandException( p_line, ee.what() ) );
			}
			catch ( VTX::CommandException & p_e )
			{
				VTX_PYTHON_OUT( "{}", p_e.what() );
			}
		}
		catch ( VTX::CommandException & p_e )
		{
			VTX_PYTHON_OUT( "{}", p_e.what() );
		}
		return {};
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

	PyTXModule & Interpretor::getModule() { return _impl->getPyTXModule(); }

	void Interpretor::print( const std::string & p_line ) const { pybind11::print( p_line ); }

	void Interpretor::getPythonModule( pybind11::module_ ** p_modulePtr ) { _impl->getPythonModule( p_modulePtr ); }

} // namespace VTX::PythonBinding
