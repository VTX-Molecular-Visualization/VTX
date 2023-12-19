#include "tools/mdprep/gromacs.hpp"
#include <dylib.hpp>
#include <util/exceptions.hpp>
#include <util/logger.hpp>
// #include <optional>

#ifndef SHARED_GROMACS
#include "tools/mdprep/gromacs/gromacs.hpp"
#endif // !SHARED_GROMACS

namespace VTX::Tool::Mdprep::Gromacs
{
	// std::optional<dylib> g_gromacs_shared_lib; // TODO : Test without reloadling lib everytime

#ifdef SHARED_GROMACS
	void submit_gromacs_command( gromacs_command_args & args )
	{
		// for now, the function load and unload the shared library each time the function is called. This is efficient
		// memory-wise, but not time-wise.

		try
		{
			dylib vtx_gromacs( "./", "vtx_gromacs" );
			auto  submit_cmd = vtx_gromacs.get_function<void( gromacs_command_args & )>( "submit_gromacs_command" );
			submit_cmd( args );
		}
		catch ( const dylib::load_error & )
		{
			VTX::VTX_ERROR( "Couldn't load gromacs shared lib" );
			throw VTX::LibException( "Shared library couldn't be found." );
		}
		catch ( const dylib::symbol_error & )
		{
			VTX::VTX_ERROR( "Couldn't locate symbol" );
			throw VTX::LibException( "Shared library was found but not the function within." );
		}
	}
#else
	void submit_gromacs_command( gromacs_command_args & args ) { ::submit_gromacs_command( args ); }

#endif
} // namespace VTX::Tool::Mdprep::Gromacs
