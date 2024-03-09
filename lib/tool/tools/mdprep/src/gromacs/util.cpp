#include "tools/mdprep/gromacs/inputs.hpp"
//
#include "tools/mdprep/gromacs/gromacs.hpp"
#include "tools/mdprep/gromacs/util.hpp"
#include <qapplication.h>
#include <util/exceptions.hpp>
#include <util/logger.hpp>
#include <util/string.hpp>

namespace VTX::Tool::Mdprep
{
	std::optional<fs::path> g_executableDirectory;
	const fs::path &		executableDirectory() noexcept
	{
		if ( !g_executableDirectory.has_value() )
		{
			// int				 argc = 0;
			// QCoreApplication app( argc, nullptr );
			// QString			 qpath = app.applicationDirPath();
			QString		qpath = QCoreApplication::applicationDirPath();
			auto		tmp	  = qpath.toLocal8Bit();
			std::string path_str( tmp.data(), tmp.size() );
			g_executableDirectory.emplace( path_str );
			g_executableDirectory->make_preferred();
		}
		return g_executableDirectory.value();
	}

} // namespace VTX::Tool::Mdprep

namespace VTX::Tool::Mdprep::Gromacs
{

	const fs::path g_defaultFfDirectoryRelativePath
		= ( fs::path( "data" ) / "tools" / "mdprep" / "gromacs" / "top" ).make_preferred();
	const fs::path & defaultFfDirectoryRelativePath() noexcept { return g_defaultFfDirectoryRelativePath; }

	const fs::path g_defaultGmxBinaryRelativePath
		= ( fs::path( "external" ) / "tools" / "mdprep" / "gromacs" / "gmx.exe" ).make_preferred();
	const fs::path & defaultGmxBinaryRelativePath() noexcept { return g_defaultGmxBinaryRelativePath; }

	void declareFfDirectory( const std::filesystem::path & p_path ) noexcept
	{
		std::string	   pathStr = p_path.string();
		QByteArrayView env_arg( pathStr.data(), pathStr.data() + pathStr.size() );
		qputenv( "GMXLIB", env_arg );
	}

	void checkJobResults( GromacsJobData & p_in ) noexcept
	{
		// TODO Check for the error pattern

		p_in.report.allOutputOk = true;
		for ( auto & it : p_in.expectedOutputFilesPtrs )
		{
			fs::path f { *it };
			if ( fs::is_regular_file( f ) == false )
			{
				p_in.report.allOutputOk = false;
				p_in.report.errors.push_back( fmt::format( "Expected output file <{}> not found.", *it ) );
				break;
			}
			if ( fs::file_size( f ) == 0 )
			{
				p_in.report.allOutputOk = false;
				p_in.report.errors.push_back( fmt::format( "Expected output file <{}> is empty.", *it ) );
				break;
			}
		}
	}

} // namespace VTX::Tool::Mdprep::Gromacs
