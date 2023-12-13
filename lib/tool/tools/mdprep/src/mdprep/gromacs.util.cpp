#include "tools/mdprep/gromacs.util.hpp"
#include <filesystem>
#include <optional>
#include <qapplication.h>

namespace VTX::Tool::Mdprep
{
	std::optional<fs::path> g_executable_directory;
	const fs::path &		executable_directory() noexcept
	{
		if ( !g_executable_directory.has_value() )
		{
			// int				 argc = 0;
			// QCoreApplication app( argc, nullptr );
			// QString			 qpath = app.applicationDirPath();
			QString		qpath = QCoreApplication::applicationDirPath();
			auto		tmp	  = qpath.toLocal8Bit();
			std::string path_str( tmp.data(), tmp.size() );
			g_executable_directory.emplace( path_str );
		}
		return g_executable_directory.value();
	}

} // namespace VTX::Tool::Mdprep

namespace VTX::Tool::Mdprep::Gromacs
{
	const fs::path g_default_ff_directory_relative_path
		= fs::path( "data" ) / "tool" / "tools" / "mdprep" / "gromacs" / "top";
	const fs::path & default_ff_directory_relative_path() noexcept { return g_default_ff_directory_relative_path; }

	void declare_ff_directory( const std::filesystem::path & path ) noexcept
	{
		std::string	   path_str = path.string();
		QByteArrayView env_arg( path_str.data(), path_str.data() + path_str.size() );
		qputenv( "GMXLIB", env_arg );
	}
	std::vector<forcefield> list_forcefields( const fs::path & ) { return {}; }

	void convert( const pdb2gmx_instructions &, gromacs_command_args & ) noexcept {}
	void convert( const solvate_instructions &, gromacs_command_args & ) noexcept {}
} // namespace VTX::Tool::Mdprep::Gromacs
