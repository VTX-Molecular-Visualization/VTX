#include "tools/mdprep/mdprep.hpp"
#include "tools/mdprep/gromacs/gromacs.hpp"
#include <dylib.hpp>
#include <filesystem>
#include <qapplication.h>
#include <util/logger.hpp>

namespace VTX::Tool::Mdprep
{
	void poc_execute_cmd( gromacs_command_args & args ) noexcept
	{
		// We need gromacs to see top param data files. One way to achieve this is to set an env var to the path of the
		// folder containing those data
		{
			int				 argc = 0;
			QCoreApplication app( argc, nullptr );
			QString			 qpath = app.applicationDirPath();
			auto			 tmp   = qpath.toLocal8Bit();
			std::string		 path_str( tmp.data(), tmp.size() );

			std::filesystem::path path( path_str );
			std::filesystem::path out_path = path / "out";
			if ( std::filesystem::is_directory( out_path ) )
				std::filesystem::remove_all( out_path );
			std::filesystem::create_directories( out_path );
			path /= "data";
			path /= "tool";
			path /= "tools";
			path /= "mdprep";
			path /= "gromacs";
			path /= "top";
			path_str = path.string();
			QByteArrayView env_arg( path_str.data(), path_str.data() + path_str.size() );
			qputenv( "GMXLIB", env_arg );
		}
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
