#include "python_binding/interpretor.hpp"
#include "python_binding/binder.hpp"
#include "python_binding/log_redirection.hpp"
#include "python_binding/wrapper/module.hpp"
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
		void initializePythonModule()
		{
			_vtxModule = pybind11::module_::import( "PyTX" );

			LogRedirection logger								= LogRedirection();
			pybind11::module_::import( "sys" ).attr( "stdout" ) = logger;

			pybind11::module_ vtxCoreModule = pybind11::module_::import( "PyTX.Core" );
			vtxCoreModule.attr( "_init" )( App::VTXApp::get().getSystemPtr() );
		}

		void addBinder( std::unique_ptr<Binder> p_binder ) { _binders.emplace_back( std::move( p_binder ) ); }

		void applyBinders()
		{
			Wrapper::Module moduleWrapper = Wrapper::Module( _vtxModule );

			for ( const std::unique_ptr<Binder> & binder : _binders )
			{
				binder->bind( moduleWrapper );
			}
		}

		void importCommands() { pybind11::exec( "from PyTX.Command import *" ); }

		pybind11::module_ & vtxModule() { return _vtxModule; }

	  private:
		pybind11::scoped_interpreter _interpretor {};
		pybind11::module_			 _vtxModule;

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
	Interpretor::~Interpretor() {}

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

	void Interpretor::print( const std::string & p_line ) const { pybind11::print( p_line ); }

	void Interpretor::runCommand( const std::string & p_line ) const
	{
		try
		{
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

} // namespace VTX::PythonBinding
