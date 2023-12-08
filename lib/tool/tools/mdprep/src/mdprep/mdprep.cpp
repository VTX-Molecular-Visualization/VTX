#include "tools/mdprep/mdprep.hpp"
#include <dylib.hpp>
#include <filesystem>
#include <qapplication.h>
#include <util/logger.hpp>

namespace vtx::tool::mdprep
{
	void poc_execute_cmd( poc_args & args ) noexcept
	{
		// We need gromacs to see top param data files. One way to achieve this is to set an env var to the path of the
		// folder containing those data
		{
			QString		qpath = QCoreApplication::applicationDirPath();
			auto		tmp	  = qpath.toLocal8Bit();
			std::string path_str( tmp.data(), tmp.size() );

			std::filesystem::path path( path_str );
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
			// auto  submit_cmd = vtx_gromacs.get_function<void()>( "simple_function" );
			// submit_cmd();
			auto submit_cmd = vtx_gromacs.get_function<void( int, char ** )>( "submit_gromacs_command" );
			//= vtx_gromacs.get_function<void( vtx::tool::mdprep::poc_args * )>( "submit_gromacs_command" );
			submit_cmd( args.i, args.s );
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

} // namespace vtx::tool::mdprep
