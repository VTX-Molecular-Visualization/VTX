#include "tools/mdprep/mdprep.hpp"
#include "tools/mdprep/gromacs.util.hpp"
#include "tools/mdprep/gromacs/gromacs.hpp"
#include <dylib.hpp>
#include <filesystem>
#include <qapplication.h>
#include <util/logger.hpp>

namespace VTX::Tool::Mdprep
{
	void poc_execute_cmd( gromacs_command_args & args ) noexcept
	{
		Gromacs::declare_ff_directory( executable_directory() / Gromacs::default_ff_directory_relative_path() );
		try
		{
			dylib vtx_gromacs( "./", "vtx_gromacs" );
			auto  submit_cmd
				= vtx_gromacs.get_function<void( VTX::Tool::Mdprep::gromacs_command_args & )>( "submit_gromacs_command"
				);
			submit_cmd( args );
		}
		catch ( const dylib::load_error & )
		{
			VTX::VTX_ERROR( "Couldn't load lib" );
		}
		catch ( const dylib::symbol_error & )
		{
			VTX::VTX_ERROR( "Couldn't locate symbol" );
		}
	}

} // namespace VTX::Tool::Mdprep
