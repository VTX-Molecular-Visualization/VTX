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
		void init()
		{
			_vtxModule = pybind11::module_::import( "PyTX" );
			pybind11::exec( "from PyTX import *" );
			_vtxModule.attr( "_init" )( App::VTXApp::get().getSystemPtr() );
		}

		pybind11::module_ & vtxModule() { return _vtxModule; }

	  private:
		pybind11::scoped_interpreter _interpretor {};
		pybind11::module_			 _vtxModule;
	};

	Interpretor::Interpretor() : _impl( std::make_unique<Interpretor::Impl>() ) { _impl->init(); }
	Interpretor::~Interpretor() {}

	void Interpretor::print( const std::string & p_line ) { pybind11::print( p_line ); }

	void Interpretor::runCommand( const std::string & p_line )
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
} // namespace VTX::PythonBinding
